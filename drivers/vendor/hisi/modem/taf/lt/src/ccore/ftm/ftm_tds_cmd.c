

/******************************************************************************
 PROJECT   :
 SUBSYSTEM :
 MODULE    :
 OWNER     :
******************************************************************************/
#include <msp_errno.h>
#include <osm.h>
#include <gen_msg.h>
#include <ftm.h>
#include "LMspLPhyInterface.h"
#include "ftm_ct.h"
#include "msp_nvim.h"
#include "msp_nv_def.h"
#include "msp_nv_id.h"
/*lint -e767*/
#define    THIS_FILE_ID        MSP_FILE_ID_FTM_TDS_CMD_C
/*lint -e767*/

#if(FEATURE_ON == FEATURE_UE_MODE_TDS)

/* 全局变量，保存校准相关信息 */
FTM_SCALIB_SYSTEM_SETTING_STRU * gst_systemSetting = NULL;
/* 全局变量，保存APC校准状态设置 */
FTM_SCALIB_APCOFFSET_SETTING_STRU * g_pstApcoffsetFlag = NULL;
/*lint -save -e762*/
extern VOS_VOID tgl00_99getCurrentCellInfo(FTM_SCELLINFO_STRU * currentCellInfo);
/*lint -restore*/

VOS_UINT32 DHI_getCurrentCellInfo(FTM_SCELLINFO_STRU * currentCellInfo)
{
    if(NULL == currentCellInfo)
    {
        return ERR_MSP_FAILURE;
    }

	tgl00_99getCurrentCellInfo(currentCellInfo);

	return ERR_MSP_SUCCESS;
}


/*****************************************************************************
 函 数 名  : atr05_51transAsciiCharIntoHex
 功能描述  : 将ASCII码字符转换为16进制数据，只识别数字与字母，
			 其它字符将被替换为0xFF
 输入参数  : VOS_CHAR
 输出参数  : 无
 返 回 值  : VOS_UINT8
*****************************************************************************/
VOS_UINT8 atr05_51transAsciiCharIntoHex(VOS_CHAR asciiChar)
{
	VOS_UINT8 v_returnVal;

	/*lint -save -e732*/
	if((asciiChar >= '0') && (asciiChar <= '9'))
	{
		v_returnVal = asciiChar - '0';
	}
	else if((asciiChar >= 'a') && (asciiChar <= 'f'))
	{
		v_returnVal = asciiChar - 'W'; /* decimal interger is 87. */
	}
	else if((asciiChar >= 'A') && (asciiChar <= 'F'))
	{
		v_returnVal = asciiChar - '7'; /* decimal interger is 55. */
	}
	/*lint -restore*/
	else
	{
		v_returnVal = 0xFF;
	}

	return v_returnVal;
}

/*****************************************************************************
 函 数 名  : atr05_72translateAsciiCharIntoTpdu
 功能描述  : 将ASCII码字符序列转换为TPDU序列
 输入参数  : asciiChar        --- 待转换序列
			 tpduResult       --- 目标序列
			 asciiCharLength  --- ASCII码字符序列长度
			 maxOutputDataLen --- 最大转换长度
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
VOS_VOID atr05_72translateAsciiCharIntoTpdu(VOS_CHAR * asciiChar, VOS_UINT8 * tpduResult,
                                           VOS_UINT16 asciiCharLength, VOS_UINT16 maxOutputDataLen)
{
	VOS_UINT16 indexOfAsciiChar = 0;
	VOS_UINT16 indexOfResult = 0;
	VOS_UINT8  v_tempVal1 = 0;
	VOS_UINT8  v_tempVal2 = 0;

	if(asciiCharLength % 2 != 0)
	{
	    asciiCharLength -= 1;
	}

	for(indexOfAsciiChar = 0, indexOfResult = 0;\
		(indexOfAsciiChar < asciiCharLength) && (indexOfResult < maxOutputDataLen);\
		indexOfAsciiChar += 2, indexOfResult++)
	{
	    v_tempVal1 = ATR_MC_ASCII_TO_HEX(asciiChar[indexOfAsciiChar]);
	    v_tempVal2 = ATR_MC_ASCII_TO_HEX(asciiChar[indexOfAsciiChar + 1]);
		/* the TPDU octet content. */
        tpduResult[indexOfResult] = (VOS_UINT8)((VOS_UINT8)((v_tempVal1 & 0x0F) << 4) | (VOS_UINT8)(v_tempVal2 & 0x0F));
    }
}

/*****************************************************************************
 函 数 名  : SystemDataTdRead
 功能描述  : 读取TDS相关NV项
 输入参数  : FTM_SCALIB_SYSTEM_SETTING_STRU
 输出参数  : 无
 返 回 值  : VOS_UINT32
*****************************************************************************/
VOS_UINT32 SystemDataTdRead(FTM_SCALIB_SYSTEM_SETTING_STRU * pstSystemInfo)
{
	VOS_UINT32 ret = ERR_MSP_SUCCESS;
    TDS_NV_PA_POWER_STRU maxPower;

    if(NULL == pstSystemInfo)
    {
        return ERR_MSP_FAILURE;
    }

    MSP_MEMSET(&maxPower, 0x00, sizeof(TDS_NV_PA_POWER_STRU));

    /* band A */
    ret = NVM_Read(NV_ID_TDS_TX_PA_POWER_BAND_A, &maxPower, AT_CALIB_PARAM_APCOFF_MAX);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }
    MSP_MEMCPY((pstSystemInfo->tdsTxPaPowerBand_A), (maxPower.asTxMaxPower), AT_CALIB_PARAM_APCOFF);

    ret = NVM_Read(NV_ID_TDS_TX_APC_COMP_BAND_A, pstSystemInfo->tdsTxApcCompBand_A, AT_CALIB_PARAM_APC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Read(NV_ID_TDS_TX_APC_FREQ_COMP_BAND_A, pstSystemInfo->tdsTxApcFreqCompBand_A, AT_CALIB_PARAM_APCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Read(NV_ID_TDS_RX_AGC_COMP_BAND_A, pstSystemInfo->tdsRxAgcCompBand_A, AT_CALIB_PARAM_AGC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Read(NV_ID_TDS_RX_AGC_FREQ_COMP_BAND_A, pstSystemInfo->tdsRxAgcFreqCompBand_A, AT_CALIB_PARAM_AGCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    /* band E */
    ret = NVM_Read(NV_ID_TDS_TX_PA_POWER_BAND_E, &maxPower, AT_CALIB_PARAM_APCOFF_MAX);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }
    MSP_MEMCPY((pstSystemInfo->tdsTxPaPowerBand_E), (maxPower.asTxMaxPower), AT_CALIB_PARAM_APCOFF);

    ret = NVM_Read(NV_ID_TDS_TX_APC_COMP_BAND_E, pstSystemInfo->tdsTxApcCompBand_E, AT_CALIB_PARAM_APC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Read(NV_ID_TDS_TX_APC_FREQ_COMP_BAND_E, pstSystemInfo->tdsTxApcFreqCompBand_E, AT_CALIB_PARAM_APCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Read(NV_ID_TDS_RX_AGC_COMP_BAND_E, pstSystemInfo->tdsRxAgcCompBand_E, AT_CALIB_PARAM_AGC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Read(NV_ID_TDS_RX_AGC_FREQ_COMP_BAND_E, pstSystemInfo->tdsRxAgcFreqCompBand_E, AT_CALIB_PARAM_AGCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    /* band F */
    ret = NVM_Read(NV_ID_TDS_TX_PA_POWER_BAND_F, &maxPower, AT_CALIB_PARAM_APCOFF_MAX);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }
    MSP_MEMCPY((pstSystemInfo->tdsTxPaPowerBand_F), (maxPower.asTxMaxPower), AT_CALIB_PARAM_APCOFF);

    ret = NVM_Read(NV_ID_TDS_TX_APC_COMP_BAND_F, pstSystemInfo->tdsTxApcCompBand_F, AT_CALIB_PARAM_APC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Read(NV_ID_TDS_TX_APC_FREQ_COMP_BAND_F, pstSystemInfo->tdsTxApcFreqCompBand_F, AT_CALIB_PARAM_APCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Read(NV_ID_TDS_RX_AGC_COMP_BAND_F, pstSystemInfo->tdsRxAgcCompBand_F, AT_CALIB_PARAM_AGC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Read(NV_ID_TDS_RX_AGC_FREQ_COMP_BAND_F, pstSystemInfo->tdsRxAgcFreqCompBand_F, AT_CALIB_PARAM_AGCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    return ret;

}

/*****************************************************************************
 函 数 名  : SystemDataTdWrite
 功能描述  : 写入TDS相关NV项
 输入参数  : FTM_SCALIB_SYSTEM_SETTING_STRU
 输出参数  : 无
 返 回 值  : VOS_UINT32
*****************************************************************************/
VOS_UINT32 SystemDataTdWrite(FTM_SCALIB_SYSTEM_SETTING_STRU * pstSystemInfo)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    TDS_NV_PA_POWER_STRU maxPower;

    if (NULL == pstSystemInfo)
    {
        return ERR_MSP_FAILURE;
    }

    MSP_MEMSET(&maxPower, 0x00, sizeof(TDS_NV_PA_POWER_STRU));

    /* band A */
    MSP_MEMCPY((maxPower.asTxMaxPower), (pstSystemInfo->tdsTxPaPowerBand_A), AT_CALIB_PARAM_APCOFF);
    ret = NVM_Write(NV_ID_TDS_TX_PA_POWER_BAND_A, &maxPower, AT_CALIB_PARAM_APCOFF_MAX);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_TX_APC_COMP_BAND_A, pstSystemInfo->tdsTxApcCompBand_A, AT_CALIB_PARAM_APC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_TX_APC_FREQ_COMP_BAND_A, pstSystemInfo->tdsTxApcFreqCompBand_A, AT_CALIB_PARAM_APCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_RX_AGC_COMP_BAND_A, pstSystemInfo->tdsRxAgcCompBand_A, AT_CALIB_PARAM_AGC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_RX_AGC_FREQ_COMP_BAND_A, pstSystemInfo->tdsRxAgcFreqCompBand_A, AT_CALIB_PARAM_AGCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    /* band E */
    MSP_MEMCPY((maxPower.asTxMaxPower), (pstSystemInfo->tdsTxPaPowerBand_E), AT_CALIB_PARAM_APCOFF);
    ret = NVM_Write(NV_ID_TDS_TX_PA_POWER_BAND_E, &maxPower, AT_CALIB_PARAM_APCOFF_MAX);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_TX_APC_COMP_BAND_E, pstSystemInfo->tdsTxApcCompBand_E, AT_CALIB_PARAM_APC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_TX_APC_FREQ_COMP_BAND_E, pstSystemInfo->tdsTxApcFreqCompBand_E, AT_CALIB_PARAM_APCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_RX_AGC_COMP_BAND_E, pstSystemInfo->tdsRxAgcCompBand_E, AT_CALIB_PARAM_AGC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_RX_AGC_FREQ_COMP_BAND_E, pstSystemInfo->tdsRxAgcFreqCompBand_E, AT_CALIB_PARAM_AGCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    /* band F */
    MSP_MEMCPY((maxPower.asTxMaxPower), (pstSystemInfo->tdsTxPaPowerBand_F), AT_CALIB_PARAM_APCOFF);
    ret = NVM_Write(NV_ID_TDS_TX_PA_POWER_BAND_F, &maxPower, AT_CALIB_PARAM_APCOFF_MAX);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_TX_APC_COMP_BAND_F, pstSystemInfo->tdsTxApcCompBand_F, AT_CALIB_PARAM_APC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_TX_APC_FREQ_COMP_BAND_F, pstSystemInfo->tdsTxApcFreqCompBand_F, AT_CALIB_PARAM_APCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_RX_AGC_COMP_BAND_F, pstSystemInfo->tdsRxAgcCompBand_F, AT_CALIB_PARAM_AGC);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

    ret = NVM_Write(NV_ID_TDS_RX_AGC_FREQ_COMP_BAND_F, pstSystemInfo->tdsRxAgcFreqCompBand_F, AT_CALIB_PARAM_AGCFREQ);
    if(ret != ERR_MSP_SUCCESS)
    {
        return ret;
    }

	return ret;
}
VOS_UINT32 SystemDataTd2Dsp(FTM_SCALIB_SYSTEM_SETTING_STRU * pstSystemInfo)
{
    if(NULL == pstSystemInfo)/* [false alarm]:fortify */
    {
        return ERR_MSP_FAILURE;/* [false alarm]:fortify */
    }

    MSP_MEMCPY((VOS_UINT8 *)(TDS_DHI_BASE_DRR), pstSystemInfo, sizeof(FTM_SCALIB_SYSTEM_SETTING_STRU));/* [false alarm]:fortify */


    return ERR_MSP_SUCCESS;/* [false alarm]:fortify */
}


VOS_UINT32 SetApcflagTd2Dsp(FTM_SCALIB_APCOFFSET_SETTING_STRU * pstSystemInfo)
{
    if(NULL == pstSystemInfo)
    {
        return ERR_MSP_FAILURE;
    }

    /* MSP_MEMCPY((VOS_VOID *)(TDS_DDR_BASE_ADDR + TDS_RFADJUSTPARA_DRR_OFFSET), pstSystemInfo, sizeof(FTM_SCALIB_APCOFFSET_SETTING_STRU));*/

	return ERR_MSP_SUCCESS;
}
VOS_UINT32 SystemDataInit(VOS_UINT32 bandType, VOS_UINT32 dataType)
{
    VOS_UINT32  v_frcId = ERR_MSP_SUCCESS;

    if(NULL == gst_systemSetting)
    {
        gst_systemSetting = (FTM_SCALIB_SYSTEM_SETTING_STRU *)VOS_MemAlloc(MSP_SYS_FTM_PID, (DYNAMIC_MEM_PT), sizeof(FTM_SCALIB_SYSTEM_SETTING_STRU));
        if(NULL == gst_systemSetting)
        {
            return ERR_MSP_NO_MORE_MEMORY;
        }

        MSP_MEMSET(gst_systemSetting, 0x00, sizeof(FTM_SCALIB_SYSTEM_SETTING_STRU));
        v_frcId = SystemDataTdRead(gst_systemSetting);
        if(ERR_MSP_SUCCESS != v_frcId)
        {
            return v_frcId;
        }
    }

    if(NULL == g_pstApcoffsetFlag)
    {
        g_pstApcoffsetFlag = (FTM_SCALIB_APCOFFSET_SETTING_STRU *)VOS_MemAlloc(MSP_SYS_FTM_PID, (DYNAMIC_MEM_PT),sizeof(FTM_SCALIB_APCOFFSET_SETTING_STRU));
        if(NULL == g_pstApcoffsetFlag)
        {
            return ERR_MSP_NO_MORE_MEMORY;
        }

        MSP_MEMSET(g_pstApcoffsetFlag, 0x00, sizeof(FTM_SCALIB_APCOFFSET_SETTING_STRU));
    }

    if(dataType != AT_CALIB_DATA_TYPE_BUTT)
    {
        switch(dataType)
        {
            case AT_CALIB_DATA_TYPE_APCOFFSETFLAG:

                MSP_MEMSET(g_pstApcoffsetFlag, 0x00, sizeof(VOS_UINT16)*2);

                /* set rf tab flag when AFC,AGC,APC */
                gst_systemSetting->tdRfTabFlag1    = 0x5555;
                gst_systemSetting->tdRfTabFlag2    = 0xAAAA;
                gst_systemSetting->tdApcOffsetFlag = 0xDCBA;

                break;

            case AT_CALIB_DATA_TYPE_APCOFFSET:

                if(AT_CALIB_BAND_TYPE_A == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxPaPowerBand_A, 0x00, AT_CALIB_PARAM_APCOFF);
                }
                else if(AT_CALIB_BAND_TYPE_E == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxPaPowerBand_E, 0x00, AT_CALIB_PARAM_APCOFF);
                }
                else if(AT_CALIB_BAND_TYPE_F == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxPaPowerBand_F, 0x00, AT_CALIB_PARAM_APCOFF);
                }
                else
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxPaPowerBand_A, 0x00, AT_CALIB_PARAM_APCOFF);
                    MSP_MEMSET(gst_systemSetting->tdsTxPaPowerBand_E, 0x00, AT_CALIB_PARAM_APCOFF);
                    MSP_MEMSET(gst_systemSetting->tdsTxPaPowerBand_F, 0x00, AT_CALIB_PARAM_APCOFF);
                }

                gst_systemSetting->tdApcOffsetFlag = 0xDCBA;


                break;

            case AT_CALIB_DATA_TYPE_APC:

                if(AT_CALIB_BAND_TYPE_A == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxApcCompBand_A, 0x00, AT_CALIB_PARAM_APC);
                }
                else if(AT_CALIB_BAND_TYPE_E == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxApcCompBand_E, 0x00, AT_CALIB_PARAM_APC);
                }
                else if(AT_CALIB_BAND_TYPE_F == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxApcCompBand_F, 0x00, AT_CALIB_PARAM_APC);
                }
                else
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxApcCompBand_A, 0x00, AT_CALIB_PARAM_APC);
                    MSP_MEMSET(gst_systemSetting->tdsTxApcCompBand_E, 0x00, AT_CALIB_PARAM_APC);
                    MSP_MEMSET(gst_systemSetting->tdsTxApcCompBand_F, 0x00, AT_CALIB_PARAM_APC);
                }

                gst_systemSetting->tdRfTabFlag1 = 0x5555;
                gst_systemSetting->tdRfTabFlag2 = 0xAAAA;

                break;

             case AT_CALIB_DATA_TYPE_APCFREQ:

                if(AT_CALIB_BAND_TYPE_A == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxApcFreqCompBand_A, 0x00, AT_CALIB_PARAM_APCFREQ);
                }
                else if(AT_CALIB_BAND_TYPE_E == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxApcFreqCompBand_E, 0x00, AT_CALIB_PARAM_APCFREQ);
                }
                else if(AT_CALIB_BAND_TYPE_F == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxApcFreqCompBand_F, 0x00, AT_CALIB_PARAM_APCFREQ);
                }
                else
                {
                    MSP_MEMSET(gst_systemSetting->tdsTxApcFreqCompBand_A, 0x00, AT_CALIB_PARAM_APCFREQ);
                    MSP_MEMSET(gst_systemSetting->tdsTxApcFreqCompBand_E, 0x00, AT_CALIB_PARAM_APCFREQ);
                    MSP_MEMSET(gst_systemSetting->tdsTxApcFreqCompBand_F, 0x00, AT_CALIB_PARAM_APCFREQ);
                }

                gst_systemSetting->tdRfTabFlag1 = 0x5555;
                gst_systemSetting->tdRfTabFlag2 = 0xAAAA;

                break;

            case AT_CALIB_DATA_TYPE_AGC:

                if(AT_CALIB_BAND_TYPE_A == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcCompBand_A, 0x00, AT_CALIB_PARAM_AGC);
                }
                else if(AT_CALIB_BAND_TYPE_E == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcCompBand_E, 0x00, AT_CALIB_PARAM_AGC);
                }
                else if(AT_CALIB_BAND_TYPE_F == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcCompBand_F, 0x00, AT_CALIB_PARAM_AGC);
                }
                else
                {
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcCompBand_A, 0x00, AT_CALIB_PARAM_AGC);
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcCompBand_E, 0x00, AT_CALIB_PARAM_AGC);
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcCompBand_F, 0x00, AT_CALIB_PARAM_AGC);
                }

                gst_systemSetting->tdRfTabFlag1 = 0x5555;
                gst_systemSetting->tdRfTabFlag2 = 0xAAAA;
                gst_systemSetting->afcAgcDcFlag = 0xABCD;

                break;

            case AT_CALIB_DATA_TYPE_AGCFREQ:

                if(AT_CALIB_BAND_TYPE_A == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcFreqCompBand_A, 0x00, AT_CALIB_PARAM_AGCFREQ);
                }
                else if(AT_CALIB_BAND_TYPE_E == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcFreqCompBand_E, 0x00, AT_CALIB_PARAM_AGCFREQ);
                }
                else if(AT_CALIB_BAND_TYPE_F == bandType)
                {
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcFreqCompBand_F, 0x00, AT_CALIB_PARAM_AGCFREQ);
                }
                else
                {
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcFreqCompBand_A, 0x00, AT_CALIB_PARAM_AGCFREQ);
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcFreqCompBand_E, 0x00, AT_CALIB_PARAM_AGCFREQ);
                    MSP_MEMSET(gst_systemSetting->tdsRxAgcFreqCompBand_F, 0x00, AT_CALIB_PARAM_AGCFREQ);
                }

                gst_systemSetting->tdRfTabFlag1 = 0x5555;
                gst_systemSetting->tdRfTabFlag2 = 0xAAAA;
                gst_systemSetting->afcAgcDcFlag = 0xABCD;

                break;

            default:

                v_frcId = ERR_MSP_FAILURE;

                break;
        }
    }
    else
    {
        MSP_MEMSET(g_pstApcoffsetFlag,  0x00, sizeof(VOS_UINT16)*2);
        MSP_MEMSET(gst_systemSetting->tdsTxPaPowerBand_A, 0x00, AT_CALIB_PARAM_APCOFF);
        MSP_MEMSET(gst_systemSetting->tdsTxPaPowerBand_E, 0x00, AT_CALIB_PARAM_APCOFF);
        MSP_MEMSET(gst_systemSetting->tdsTxPaPowerBand_F, 0x00, AT_CALIB_PARAM_APCOFF);
        MSP_MEMSET(gst_systemSetting->tdsTxApcCompBand_A, 0x00, AT_CALIB_PARAM_APC);
        MSP_MEMSET(gst_systemSetting->tdsTxApcCompBand_E, 0x00, AT_CALIB_PARAM_APC);
        MSP_MEMSET(gst_systemSetting->tdsTxApcCompBand_F, 0x00, AT_CALIB_PARAM_APC);
        MSP_MEMSET(gst_systemSetting->tdsTxApcFreqCompBand_A, 0x00, AT_CALIB_PARAM_APCFREQ);
        MSP_MEMSET(gst_systemSetting->tdsTxApcFreqCompBand_E, 0x00, AT_CALIB_PARAM_APCFREQ);
        MSP_MEMSET(gst_systemSetting->tdsTxApcFreqCompBand_F, 0x00, AT_CALIB_PARAM_APCFREQ);
        MSP_MEMSET(gst_systemSetting->tdsRxAgcCompBand_A, 0x00, AT_CALIB_PARAM_AGC);
        MSP_MEMSET(gst_systemSetting->tdsRxAgcCompBand_E, 0x00, AT_CALIB_PARAM_AGC);
        MSP_MEMSET(gst_systemSetting->tdsRxAgcCompBand_F, 0x00, AT_CALIB_PARAM_AGC);
        MSP_MEMSET(gst_systemSetting->tdsRxAgcFreqCompBand_A, 0x00, AT_CALIB_PARAM_AGCFREQ);
        MSP_MEMSET(gst_systemSetting->tdsRxAgcFreqCompBand_E, 0x00, AT_CALIB_PARAM_AGCFREQ);
        MSP_MEMSET(gst_systemSetting->tdsRxAgcFreqCompBand_F, 0x00, AT_CALIB_PARAM_AGCFREQ);
        gst_systemSetting->afcAgcDcFlag     = 0xABCD;
        gst_systemSetting->tdApcOffsetFlag  = 0xDCBA;
        gst_systemSetting->tdRfTabFlag1     = 0x5555;
        gst_systemSetting->tdRfTabFlag2     = 0xAAAA;
    }

    return v_frcId;
}
VOS_VOID StartRFAdjust(VOS_VOID)
{
}


VOS_VOID FinishRFAdjust(VOS_VOID)
{
}


VOS_UINT32 SetApcoffsetFlag(VOS_CHAR * calibData)
{
    VOS_UINT8 a_converArray[sizeof(FTM_SCALIB_APCOFFSET_SETTING_STRU)];

    if(NULL == g_pstApcoffsetFlag || NULL == calibData)
    {
        return ERR_MSP_FAILURE;
    }

    MSP_MEMSET(a_converArray, 0x00, sizeof(FTM_SCALIB_APCOFFSET_SETTING_STRU));

    ATR_MC_ASCII_TO_TPDU(calibData, a_converArray, (VOS_UINT16)VOS_StrLen(calibData), sizeof(FTM_SCALIB_APCOFFSET_SETTING_STRU) * 2 + 1);
    MSP_MEMCPY(g_pstApcoffsetFlag, a_converArray, sizeof(FTM_SCALIB_APCOFFSET_SETTING_STRU));

    return ERR_MSP_SUCCESS;
}


VOS_UINT32 SetCalibParamInBuffer(VOS_UINT32 bandType, VOS_UINT32 dataType, VOS_CHAR * calibData, VOS_UINT16 dataLen)
{
    VOS_UINT8 a_converArray[AT_CALIB_PARAM_APC];

    MSP_MEMSET(a_converArray, 0x00, AT_CALIB_PARAM_APC);

    if(NULL == calibData)
    {
        return ERR_MSP_FAILURE;
    }

    if(dataType == AT_CALIB_DATA_TYPE_BUTT)
    {
        return ERR_MSP_FAILURE;
    }

    switch(dataType)
    {
        case AT_CALIB_DATA_TYPE_APCOFFSETFLAG:

            /* APCOFFSETFLAG只通过SET命令设置 */
            return ERR_MSP_FAILURE;

        case AT_CALIB_DATA_TYPE_APCOFFSET:

            /* 数据超过最大长度 */
            if(dataLen > AT_CALIB_PARAM_APCOFF*2)
            {
                return ERR_MSP_FAILURE;
            }

            ATR_MC_ASCII_TO_TPDU(calibData, a_converArray, (VOS_UINT16)VOS_StrLen(calibData), AT_CALIB_PARAM_APCOFF * 2 + 1);

            if(AT_CALIB_BAND_TYPE_A == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsTxPaPowerBand_A, a_converArray, AT_CALIB_PARAM_APCOFF);
            }
            else if(AT_CALIB_BAND_TYPE_E == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsTxPaPowerBand_E, a_converArray, AT_CALIB_PARAM_APCOFF);
            }
            else if(AT_CALIB_BAND_TYPE_F == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsTxPaPowerBand_F, a_converArray, AT_CALIB_PARAM_APCOFF);
            }
            else
            {
                return ERR_MSP_FAILURE;
            }

            gst_systemSetting->tdApcOffsetFlag = 0xDCBA;

            break;

        case AT_CALIB_DATA_TYPE_APC:

            /* 数据超过最大长度 */
            if(dataLen > AT_CALIB_PARAM_APC*2)
            {
                return ERR_MSP_FAILURE;
            }

            ATR_MC_ASCII_TO_TPDU(calibData, a_converArray, (VOS_UINT16)VOS_StrLen(calibData), AT_CALIB_PARAM_APC * 2 + 1);

            if(AT_CALIB_BAND_TYPE_A == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsTxApcCompBand_A, a_converArray, AT_CALIB_PARAM_APC);
            }
            else if(AT_CALIB_BAND_TYPE_E == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsTxApcCompBand_E, a_converArray, AT_CALIB_PARAM_APC);
            }
            else if(AT_CALIB_BAND_TYPE_F == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsTxApcCompBand_F, a_converArray, AT_CALIB_PARAM_APC);
            }
            else
            {
                return ERR_MSP_FAILURE;
            }

            gst_systemSetting->tdRfTabFlag1 = 0x5555;
            gst_systemSetting->tdRfTabFlag2 = 0xAAAA;

            break;

         case AT_CALIB_DATA_TYPE_APCFREQ:

            /* 数据超过最大长度 */
            if(dataLen > AT_CALIB_PARAM_APCFREQ*2)
            {
                return ERR_MSP_FAILURE;
            }

            ATR_MC_ASCII_TO_TPDU(calibData, a_converArray, (VOS_UINT16)VOS_StrLen(calibData), AT_CALIB_PARAM_APCFREQ * 2 + 1);

            if(AT_CALIB_BAND_TYPE_A == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsTxApcFreqCompBand_A, a_converArray, AT_CALIB_PARAM_APCFREQ);
            }
            else if(AT_CALIB_BAND_TYPE_E == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsTxApcFreqCompBand_E, a_converArray, AT_CALIB_PARAM_APCFREQ);
            }
            else if(AT_CALIB_BAND_TYPE_F == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsTxApcFreqCompBand_F, a_converArray, AT_CALIB_PARAM_APCFREQ);
            }
            else
            {
                return ERR_MSP_FAILURE;
            }

            gst_systemSetting->tdRfTabFlag1 = 0x5555;
            gst_systemSetting->tdRfTabFlag2 = 0xAAAA;

            break;

        case AT_CALIB_DATA_TYPE_AGC:

            /* 数据超过最大长度 */
            if(dataLen > AT_CALIB_PARAM_AGC*2)
            {
                return ERR_MSP_FAILURE;
            }

            ATR_MC_ASCII_TO_TPDU(calibData, a_converArray, (VOS_UINT16)VOS_StrLen(calibData), AT_CALIB_PARAM_AGC * 2 + 1);

            if(AT_CALIB_BAND_TYPE_A == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsRxAgcCompBand_A, a_converArray, AT_CALIB_PARAM_AGC);
            }
            else if(AT_CALIB_BAND_TYPE_E == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsRxAgcCompBand_E, a_converArray, AT_CALIB_PARAM_AGC);
            }
            else if(AT_CALIB_BAND_TYPE_F == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsRxAgcCompBand_F, a_converArray, AT_CALIB_PARAM_AGC);
            }
            else
            {
                return ERR_MSP_FAILURE;
            }

            gst_systemSetting->tdRfTabFlag1 = 0x5555;
            gst_systemSetting->tdRfTabFlag2 = 0xAAAA;
            gst_systemSetting->afcAgcDcFlag = 0xABCD;

            break;

        case AT_CALIB_DATA_TYPE_AGCFREQ:

            /* 数据超过最大长度 */
            if(dataLen > AT_CALIB_PARAM_AGCFREQ*2)
            {
                return ERR_MSP_FAILURE;
            }

            ATR_MC_ASCII_TO_TPDU(calibData, a_converArray, (VOS_UINT16)VOS_StrLen(calibData), AT_CALIB_PARAM_AGCFREQ * 2 + 1);

            if(AT_CALIB_BAND_TYPE_A == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsRxAgcFreqCompBand_A, a_converArray, AT_CALIB_PARAM_AGCFREQ);
            }
            else if(AT_CALIB_BAND_TYPE_E == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsRxAgcFreqCompBand_E, a_converArray, AT_CALIB_PARAM_AGCFREQ);
            }
            else if(AT_CALIB_BAND_TYPE_F == bandType)
            {
                MSP_MEMCPY(gst_systemSetting->tdsRxAgcFreqCompBand_F, a_converArray, AT_CALIB_PARAM_AGCFREQ);
            }
            else
            {
                return ERR_MSP_FAILURE;
            }

            gst_systemSetting->tdRfTabFlag1 = 0x5555;
            gst_systemSetting->tdRfTabFlag2 = 0xAAAA;
            gst_systemSetting->afcAgcDcFlag = 0xABCD;

            break;

        default:

            return ERR_MSP_FAILURE;

    }

    return ERR_MSP_SUCCESS;
}


VOS_UINT32 SetScalibPara(FTM_SET_SCALIB_REQ_STRU * pstFtmReq, FTM_SET_SCALIB_CNF_STRU * pstCnf)
{
    VOS_UINT32 v_frcId    = ERR_MSP_FAILURE;
    VOS_UINT32 v_opType;
    VOS_UINT32 v_bandType;
    VOS_UINT32 v_dataType;
	/*lint -save -e813*/
    FTM_SCALIB_SYSTEM_SETTING_STRU s_systemSetting;
	/*lint -restore*/
    if(NULL == pstCnf || NULL == pstFtmReq)
    {
        return ERR_MSP_FAILURE;
    }

    v_opType   = pstFtmReq->tds_OpType;
    v_bandType = pstFtmReq->tds_BandType;
    v_dataType = pstFtmReq->tds_DataType;

    /* 返回消息结构初始化 */
    pstCnf->tds_OpType   = v_opType;
    pstCnf->tds_BandType = v_bandType;
    pstCnf->tds_DataType = v_dataType;
    MSP_MEMSET(&(pstCnf->systemSetting), 0x00, sizeof(FTM_SCALIB_SYSTEM_SETTING_STRU));
    MSP_MEMSET(&(pstCnf->apcoffsetFlag), 0x00, sizeof(FTM_SCALIB_APCOFFSET_SETTING_STRU));

    switch (v_opType)
	{
        /* 初始化缓存 */
        case AT_CALIB_OP_TYPE_INI:

            v_frcId = SystemDataInit(v_bandType, v_dataType);

			break;

        /* 数据写入缓存 */
        case AT_CALIB_OP_TYPE_CACHE:

            if(gst_systemSetting != NULL)
            {
                if(pstFtmReq->tds_DataLen != 0)
                {
                    v_frcId = SetCalibParamInBuffer(v_bandType, v_dataType, (VOS_CHAR *)pstFtmReq->tds_Data, pstFtmReq->tds_DataLen);
                }
                else
                {
                    /* 写入指定类型数据，数据为空 */
                    v_frcId = ERR_MSP_FAILURE;
                }
            }
            else
            {
                /* 在写入缓存之前必须初始化缓存 */
                return ERR_MSP_FAILURE;
            }

            break;

		/* 将已缓存数据写入共享内存，并通知DSP立即加载使用 */
        case AT_CALIB_OP_TYPE_USE:

			if(gst_systemSetting != NULL)
			{
				if(pstFtmReq->tds_DataLen!= 0)
				{
					v_frcId = SetCalibParamInBuffer(v_bandType, v_dataType, (VOS_CHAR *)pstFtmReq->tds_Data, pstFtmReq->tds_DataLen);
				}
                else
                {
                    /* 写入指定类型数据，数据为空 */
                    v_frcId = ERR_MSP_FAILURE;
                }

                if(ERR_MSP_SUCCESS != v_frcId)
                {
                    return v_frcId;
                }

				v_frcId = SystemDataTd2Dsp(gst_systemSetting);
			}

			break;

        /* 将已接收数据写入FLASH，释放接收缓存 */
        case AT_CALIB_OP_TYPE_SAVE:

            if(gst_systemSetting != NULL)
            {
                /* 数据不为空时，需先将数据写入缓存 */
                if(pstFtmReq->tds_DataLen != 0)
                {
                    v_frcId = SetCalibParamInBuffer(v_bandType, v_dataType, (VOS_CHAR *)pstFtmReq->tds_Data, (VOS_UINT16)pstFtmReq->tds_DataLen);
                }

                /* 只有操作类型的情况下，也需把缓存数据写入FLASH */
                if((ERR_MSP_SUCCESS != v_frcId) &&
                    ((AT_CALIB_DATA_TYPE_BUTT != v_dataType) || (AT_CALIB_BAND_TYPE_BUTT != v_bandType)))
                {
                    return v_frcId;
                }

                v_frcId = SystemDataTdWrite(gst_systemSetting);
                VOS_MemFree(MSP_SYS_FTM_PID, gst_systemSetting);
                gst_systemSetting = NULL;
                VOS_MemFree(MSP_SYS_FTM_PID, g_pstApcoffsetFlag);
                g_pstApcoffsetFlag = NULL;
            }

            break;

		/* 查询当前缓存中指定类型的数据 */
        case AT_CALIB_OP_TYPE_GET:

            if(gst_systemSetting != NULL && g_pstApcoffsetFlag != NULL)
            {
				MSP_MEMCPY(&(pstCnf->systemSetting), gst_systemSetting, sizeof(FTM_SCALIB_SYSTEM_SETTING_STRU));
                MSP_MEMCPY(&(pstCnf->apcoffsetFlag), g_pstApcoffsetFlag, sizeof(FTM_SCALIB_APCOFFSET_SETTING_STRU));
                v_frcId = ERR_MSP_SUCCESS;
            }
            else
            {
            }

            break;

		/* 从FLASH中读取数据 */
		case AT_CALIB_OP_TYPE_READ:

            /* READ操作不能读取APCOFFSETFLAG数据 */
            if(AT_CALIB_DATA_TYPE_APCOFFSETFLAG == v_dataType)
            {
                return ERR_MSP_FAILURE;
            }

            MSP_MEMSET(&s_systemSetting, 0x00, sizeof(FTM_SCALIB_SYSTEM_SETTING_STRU));
            v_frcId = SystemDataTdRead(&s_systemSetting);

    		if(ERR_MSP_SUCCESS == v_frcId)
			{
                MSP_MEMCPY(&(pstCnf->systemSetting), &s_systemSetting, sizeof(FTM_SCALIB_SYSTEM_SETTING_STRU));
			}
            else
            {
            }

            break;

        /* 开始校准 */
        case AT_CALIB_OP_TYPE_BEGIN:

            StartRFAdjust();
            v_frcId = ERR_MSP_SUCCESS;

			break;

        /* 结束校准 */
        case AT_CALIB_OP_TYPE_END:

            FinishRFAdjust();
            v_frcId = ERR_MSP_SUCCESS;

			break;

        /* 设置DSP校准状态 */
        case AT_CALIB_OP_TYPE_SET:

            /* SET命令只支持APCOFFSETFLAG的设置 */
            if(AT_CALIB_DATA_TYPE_APCOFFSETFLAG != v_dataType)
            {
                v_frcId = ERR_MSP_INVALID_PARAMETER;
                break;
            }

            /* 长度检查 */
            if(0 == pstFtmReq->tds_DataLen || pstFtmReq->tds_DataLen > sizeof(VOS_UINT32)*2)
            {
                v_frcId = ERR_MSP_FAILURE;
                break;
            }

            v_frcId = SetApcoffsetFlag((VOS_CHAR *)pstFtmReq->tds_Data);
            if(ERR_MSP_SUCCESS != v_frcId)
            {
                return v_frcId;
            }

            v_frcId = SetApcflagTd2Dsp(g_pstApcoffsetFlag);

			break;

        default:

            v_frcId = ERR_MSP_FAILURE;

            break;
    }

    return v_frcId;
}



VOS_UINT32 at_ftm_scalibRdFun(VOS_VOID* pParam)
{
    VOS_UINT32 ret                        = ERR_MSP_SUCCESS;
    FTM_RD_SCALIB_CNF_STRU  * pstCnf = NULL;

    pstCnf = (FTM_RD_SCALIB_CNF_STRU *)VOS_MemAlloc(MSP_SYS_FTM_PID, (DYNAMIC_MEM_PT), sizeof(FTM_RD_SCALIB_CNF_STRU));
    
    if (NULL != pstCnf)
    {
        if (NULL == gst_systemSetting)
        {
            pstCnf->ulErrCode = ERR_MSP_FAILURE;
        }
        else
        {
            pstCnf->ulErrCode = ERR_MSP_SUCCESS;
        }
        ret = ftm_SendDataMsg(MSP_SYS_FTM_PID, WUEPS_PID_AT, ID_MSG_FTM_RD_SCALIB_CNF,
            (VOS_UINT32) pstCnf, sizeof(FTM_RD_SCALIB_CNF_STRU));
        VOS_MemFree(MSP_SYS_FTM_PID, pstCnf);
    }
	else
	{
		ret = ERR_MSP_NO_MORE_MEMORY;
	}

    return ret;
}


VOS_UINT32 at_ftm_scalibSetFun(VOS_VOID* pParam)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
    FTM_SET_SCALIB_REQ_STRU * pstFtmReq = (FTM_SET_SCALIB_REQ_STRU *)pParam;
	/*lint -save -e813*/
    FTM_SET_SCALIB_CNF_STRU   stCnf     = { 0 };
	/*lint -restore*/

    ret = SetScalibPara(pstFtmReq, &stCnf);
    stCnf.ulErrCode = ret;
    return ftm_SendDataMsg(MSP_SYS_FTM_PID, WUEPS_PID_AT, ID_MSG_FTM_SET_SCALIB_CNF,
                        (VOS_UINT32)&stCnf, sizeof(FTM_SET_SCALIB_CNF_STRU));
}


VOS_UINT32 at_ftm_scellinfoRdFun(VOS_VOID* pParam)
{
    VOS_UINT32 ret                           = ERR_MSP_SUCCESS;
    FTM_RD_SCELLINFO_CNF_STRU  * pstCnf = NULL;

    pstCnf = (FTM_RD_SCELLINFO_CNF_STRU *)VOS_MemAlloc(MSP_SYS_FTM_PID ,(DYNAMIC_MEM_PT), sizeof(FTM_RD_SCELLINFO_CNF_STRU));

    if (NULL != pstCnf)
    {
        MSP_MEMSET(pstCnf, 0x00, sizeof(FTM_RD_SCELLINFO_CNF_STRU));
        ret = DHI_getCurrentCellInfo(&(pstCnf->scellinfoStru));
        pstCnf->ulErrCode = ret;
        ret = ftm_SendDataMsg(MSP_SYS_FTM_PID, WUEPS_PID_AT, ID_MSG_FTM_RD_SCELLINFO_CNF,
            (VOS_UINT32) pstCnf, sizeof(FTM_RD_SCELLINFO_CNF_STRU));
        VOS_MemFree(MSP_SYS_FTM_PID, pstCnf);
    }
	else
	{
		ret = ERR_MSP_NO_MORE_MEMORY;
	}

    return ret;
}
#endif


/*****************************************************************************
 函 数 名  : at_ftm_cmtm_read
 功能描述  : ID_MSG_FTM_RD_TEMPERATURE_REQ 处理函数
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功返回ERR_MSP_SUCCESS, 否则返回其他结果
*****************************************************************************/
VOS_UINT32 at_ftm_cmtm_read(VOID* pParam)
{
    VOS_UINT32 ret                           = ERR_MSP_SUCCESS;
    VOS_UINT16 temperature                   =   0;
	/*lint -save -e958*/
	FTM_RD_TEMPERATURE_CNF_STRU stRdCnf = { 0 };
    /*lint -restore*/
    ret = (VOS_UINT32)BSP_HKADC_PaValueGet(&temperature);
    if (ERR_MSP_SUCCESS == ret)
    {
        stRdCnf.ulErrCode     = ERR_MSP_SUCCESS;
        stRdCnf.ulTemperature = (VOS_UINT32)temperature;
    }
    else
    {
        stRdCnf.ulErrCode = ERR_MSP_FAILURE;
    }

    return ftm_comm_send(ID_MSG_FTM_RD_TEMPERATURE_CNF, (VOS_UINT32)&stRdCnf, sizeof(FTM_RD_TEMPERATURE_CNF_STRU));
}

