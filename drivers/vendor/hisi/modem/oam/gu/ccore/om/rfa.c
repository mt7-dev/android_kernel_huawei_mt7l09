/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : RFA.C
  版 本 号   : 初稿
  作    者   : 李霄 46160
  生成日期   : 2008年2月1日
  最近修改   :
  功能描述   : RF校准模块
  函数列表   :
  修改历史   :
  1.日    期   : 2008年2月1日
    作    者   : 李霄 46160
    修改内容   : 创建文件

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
/*lint -e767 修改人：m00128685；检视人：l46160；原因简述：打点日志文件宏ID定义*/
#define    THIS_FILE_ID        PS_FILE_ID_RFA_C
/*lint +e767 修改人：m00128685；检视人：l46160*/


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "rfa.h"
#include "om.h"

/* CQI表NV项读取列表 */ /* V3R3SFT 联调删除掉表456和EEIC相关的表 */
VOS_UINT16 g_ausCQINvIdReadList[]=
{
    en_NV_Item_W_HSPA_CQI_TABLE1,
    en_NV_Item_W_HSPA_CQI_TABLE2,
    en_NV_Item_W_HSPA_CQI_TABLE3,
};


/*****************************************************************************
 函 数 名  : RFA_GetFixMemInfo
 功能描述  : 完成DSP所需的内存信息获取，并且输出物理地址
 输入参数  : ulMemType: 内存类型，输入底软的宏定义
             ulMemSize: 内存空间，现在未使用，后续底软动态分配的时候需要
 输出参数  : pulMemAddr:内存的地址
 返 回 值  : VOS_OK/VOS_ERR
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年3月8日
    作    者   : zhuli
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 RFA_GetFixMemInfo(VOS_UINT32 ulMemType, VOS_UINT32 ulMemSize, VOS_UINT32 *pulMemAddr)
{
    BSP_DDR_SECT_QUERY  stDDRQuery;
    BSP_DDR_SECT_INFO   stDDRInfo;

    /*填写内存的相关信息*/
    stDDRQuery.enSectType   = (BSP_DDR_SECT_TYPE_E)ulMemType;
    stDDRQuery.enSectAttr   = BSP_DDR_SECT_ATTR_NONCACHEABLE;
    stDDRQuery.enPVAddr     = BSP_DDR_SECT_PVADDR_EQU;
    stDDRQuery.ulSectSize   = ulMemSize;

    /*获取当前的内存信息*/
    if(BSP_OK != DRV_GET_FIX_DDR_ADDR(&stDDRQuery, &stDDRInfo))
    {
        PS_LOG1( UEPS_PID_APM, 0, PS_PRINT_ERROR," SHPA_GetMemInfoForDSP:DRV Get Mem %d Info Error.\n",(VOS_INT32)ulMemType);

        return VOS_ERR;     /*异常退出*/
    }

    *pulMemAddr = (VOS_UINT32)stDDRInfo.pSectVirtAddr; /*由于虚实地址一致，因此返回其一即可*/

    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RFA_Init
 功能描述  : RFA模块初始化
 输入参数  : 无


 输出参数  : 无
 返 回 值  :  VOS_UINT32
 调用函数  :
 被调函数  :

 修改历史  :

*****************************************************************************/
VOS_UINT32 RFA_Init(VOS_VOID)
{
    return VOS_OK;
}

/*****************************************************************************
 函 数 名  : RFA_MsgProc
 功能描述  : RFA模块的消息处理函数
 输入参数  : pMsg  消息块


 输出参数  : 无
 返 回 值  : 成功，失败
 调用函数  :
 被调函数  :

 修改历史  :

*****************************************************************************/
VOS_UINT32 RFA_MsgProc(MsgBlock *pMsg)
{
    return VOS_OK;
}


/*****************************************************************************
 函 数 名  : RFA_CQITableProc
 功能描述  : RFA模块的CQI表处理函数
 输入参数  : 无


 输出参数  : 无
 返 回 值  : 成功，失败
 调用函数  :
 被调函数  :

 修改历史  :

*****************************************************************************/
VOS_UINT32 RFA_CQITableProc(VOS_VOID)
{
    VOS_UINT32                  i;
    VOS_UINT32                  ulMemAddr;
    VOS_UINT8                   *pucTemp;

    if(BSP_OK != RFA_GetFixMemInfo(BSP_DDR_SECT_TYPE_CQI, RFA_CQI_TOTAL_LENGTH, &ulMemAddr))
    {
        PS_LOG(WUEPS_PID_RFA, 0, PS_PRINT_ERROR, "RFA_CQITableProc:Get CQI Memory Fail.\n");

        return VOS_ERR;
    }

    pucTemp = (VOS_UINT8 *)ulMemAddr;    /*ARM_DSP_CQI_TBL_BASE_ADDR*/

    /* 读取CQI表NV 项并填入到CQI表内存中*/
    /* EEIC使能开关的值usEEICMode作为数组行下标，0:非EEIC CQI表 1:EEIC CQI 表*/
    for ( i = 0; i < sizeof(g_ausCQINvIdReadList)/sizeof(VOS_UINT16); i++)
    {

        if(NV_OK != NV_Read(g_ausCQINvIdReadList[i], pucTemp, RFA_CQI_TABLE_LENGTH))
        {
            PS_LOG1(WUEPS_PID_RFA, 0, PS_PRINT_ERROR, "RFA_CQITableProc: NV %d Read fail.\n"
                                    , g_ausCQINvIdReadList[i]);

            return VOS_ERR;
        }

        pucTemp += RFA_CQI_TABLE_LENGTH;

    }

    return VOS_OK;
}



/*****************************************************************************
 函 数 名  : WuepsRfaPidInit
 功能描述  :
 输入参数  : 无


 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :

 修改历史  :

*****************************************************************************/
VOS_UINT32 WuepsRfaPidInit ( enum VOS_INIT_PHASE_DEFINE ip )
{
    switch( ip )
    {
        case   VOS_IP_LOAD_CONFIG:

               RFA_Init();

               break;
        case   VOS_IP_FARMALLOC:
        case   VOS_IP_INITIAL:
        case   VOS_IP_ENROLLMENT:
        case   VOS_IP_LOAD_DATA:
        case   VOS_IP_FETCH_DATA:
        case   VOS_IP_STARTUP:
        case   VOS_IP_RIVAL:
        case   VOS_IP_KICKOFF:
        case   VOS_IP_STANDBY:
        case   VOS_IP_BROADCAST_STATE:
        case   VOS_IP_RESTART:
        case   VOS_IP_BUTT:
               break;
    }
    return VOS_OK;
}

#if 0
/********************************************************************
  Function:         RFA_AptNVListProc
  Description:      APT NV项读取
  Input:
  Output:
  Return:           VOS_OK/ VOS_ERR
  Others:
********************************************************************/
VOS_UINT32 RFA_AptNVListProc(VOS_UINT16* pusAPTNvIdList, RFA_APT_NV_DATA_STRU* pstAptNVItem)
{
    VOS_UINT32   i;
    VOS_UINT32   ulTempCount;
    VOS_INT16    asAPTCompHC[APT_NV_DATA_MAX_NUM];
    VOS_INT16    asAPTCompMC[APT_NV_DATA_MAX_NUM];
    VOS_INT16    asAPTCompLC[APT_NV_DATA_MAX_NUM];

    RFA_APT_NV_VBIAS_STRU stAptVbias;

    if(NV_OK != NV_Read((*pusAPTNvIdList++), &stAptVbias, sizeof(stAptVbias)))
    {
        PS_LOG1(WUEPS_PID_RFA, 0, PS_PRINT_ERROR, "RFA_AptNVListProc: NV %d Read fail.\n",*pusAPTNvIdList);

        return VOS_ERR;
    }

    if(NV_OK != NV_Read((*pusAPTNvIdList++), asAPTCompHC, sizeof(asAPTCompHC)))
    {
        PS_LOG1(WUEPS_PID_RFA, 0, PS_PRINT_ERROR, "RFA_AptNVListProc: NV %d Read fail.\n",*pusAPTNvIdList);

        return VOS_ERR;
    }

    if(NV_OK != NV_Read((*pusAPTNvIdList++), asAPTCompMC, sizeof(asAPTCompMC)))
    {
        PS_LOG1(WUEPS_PID_RFA, 0, PS_PRINT_ERROR, "RFA_AptNVListProc: NV %d Read fail.\n",*pusAPTNvIdList);

        return VOS_ERR;
    }

    if(NV_OK != NV_Read((*pusAPTNvIdList), asAPTCompLC, sizeof(asAPTCompLC)))
    {
        PS_LOG1(WUEPS_PID_RFA, 0, PS_PRINT_ERROR, "RFA_AptNVListProc: NV %d Read fail.\n",*pusAPTNvIdList);

        return VOS_ERR;
    }

    if (( APT_NV_DATA_MAX_NUM < stAptVbias.ulCount ) || ( APT_NV_DATA_MIN_NUM == stAptVbias.ulCount) )
    {
        PS_LOG(WUEPS_PID_RFA, 0, PS_PRINT_ERROR, "RFA_AptNVListProc: check NV fail.\n");

        return VOS_ERR;
    }

    ulTempCount = stAptVbias.ulCount;

    pstAptNVItem->ulCount = ulTempCount;

    /*NV项中功率从大到小，读取NV项数据做一次翻转*/
    for(i=0; i< ulTempCount; i++)
    {
        pstAptNVItem->asNvPower[i]   = stAptVbias.asNvItem[ulTempCount-1-i][0];
        pstAptNVItem->ausNvVolt[i]   = (VOS_UINT16)stAptVbias.asNvItem[ulTempCount-1-i][1];
        pstAptNVItem->asNvCompHC[i]  = asAPTCompHC[ulTempCount-1-i];
        pstAptNVItem->asNvCompMC[i]  = asAPTCompMC[ulTempCount-1-i];
        pstAptNVItem->asNvCompLC[i]  = asAPTCompLC[ulTempCount-1-i];
    }

    /*对临界值赋初值*/
    pstAptNVItem->ausNvVolt[ulTempCount]   = (VOS_UINT16)stAptVbias.asNvItem[0][1];
    pstAptNVItem->asNvCompHC[ulTempCount]  = asAPTCompHC[0];
    pstAptNVItem->asNvCompMC[ulTempCount]  = asAPTCompMC[0];
    pstAptNVItem->asNvCompLC[ulTempCount]  = asAPTCompLC[0];

    return VOS_OK;
}

VOS_UINT32 RFA_ProcAptNvItem(VOS_INT16 *pItem, RFA_APT_NV_DATA_STRU stAptNVData , VOS_UINT32 ulGainMode )
{
    VOS_UINT32               i;
    VOS_UINT32               j;
    VOS_UINT32               ulCount;
    VOS_INT16                sTempOffset;
    VOS_INT16*               psAPTVbias;
    VOS_INT16*               psAPTHCComp;
    VOS_INT16*               psAPTMCComp;
    VOS_INT16*               psAPTLCComp;

    VOS_UINT16               ausNvPowerOffset[APT_NV_DATA_MAX_NUM+2];
    VOS_INT16                sPowerBase[] = {
                                                RFA_APT_Table_HIGH_GAIN_Base_P0,
                                                RFA_APT_Table_MID_GAIN_Base_P0,
                                                RFA_APT_Table_LOW_GAIN_Base_P0
                                            };

    /*计算V16、V15、V14……V1对应APT表的地址：(P0+P16)/8、(P0+P15)/8、(P0+P3)/8、(P0+P1)/8*/
    ausNvPowerOffset[0] = 0;
    ulCount=stAptNVData.ulCount;

    for(i=0; i<ulCount; i++)
    {
        sTempOffset = stAptNVData.asNvPower[i]+sPowerBase[ulGainMode];

        /*对偏移量作合法性检测*/
        if(0 > sTempOffset )
        {
            return VOS_ERR;
        }

        /*扩展倍数为8  */
        ausNvPowerOffset[i+1]=(VOS_UINT16)sTempOffset>>3;
    }

    /*解决临界情况,,对偏移量超过或等于128作保护*/
    /*数组ausNvPowerOffset保存地址从0~128的所有地址*/
    if ( (RFA_APT_ITEM_SIZE) < ausNvPowerOffset[ulCount] )
    {
        ausNvPowerOffset[ulCount] = RFA_APT_ITEM_SIZE;
    }

    ausNvPowerOffset[ulCount+1] = RFA_APT_ITEM_SIZE;

    psAPTVbias  =  pItem;
    psAPTHCComp =  psAPTVbias + RFA_APT_GAIN_OFFSET;
    psAPTMCComp =  psAPTHCComp + RFA_APT_GAIN_OFFSET;
    psAPTLCComp =  psAPTMCComp + RFA_APT_GAIN_OFFSET;

    /*地址从0~(P0+P16)/8的表中填入V16，地址从(P0+P16)/8~(P0+P15)/8*/
    /*的表中填入V15.以此类推，地址从(P0+P1)/8~127的表中填入V1*/
    for ( i = 0; i < ulCount+1; i++)
    {
        for ( j = ausNvPowerOffset[i]; j < ausNvPowerOffset[i+1]; j++ )
        {
            *(psAPTVbias+j)   = (VOS_INT16)stAptNVData.ausNvVolt[i];
            *(psAPTHCComp+j)  = stAptNVData.asNvCompHC[i];
            *(psAPTMCComp+j)  = stAptNVData.asNvCompMC[i];
            *(psAPTLCComp+j)  = stAptNVData.asNvCompLC[i];
        }

    }

    return VOS_OK;
}
#endif

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

