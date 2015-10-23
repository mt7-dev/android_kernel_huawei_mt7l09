
/*lint  --e{537,740,438,767,732}*/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/

#include "osm.h"
#include "msp_errno.h"
#include "bbp_datalog.h"
#include "diag_agent.h"
#include "diag_common.h"
#include "diag_cmd_id.h"
#include "diag_debug.h"

#define    THIS_FILE_ID   MSP_FILE_ID_BBP_AGENT_C



/*****************************************************************************
  2 Declare the Global Variable
*****************************************************************************/


/*****************************************************************************
  3 Function
*****************************************************************************/

VOS_SEM g_ulDataSampleIndSem;
VOS_BOOL g_blBbpAgentSleepFlag = 0;

DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU *g_pDataSampleRecordBuf = NULL;
DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU *g_pLogSampleRecordBuf[BBP_LOG_SAMPLE_MAX_CHAN_NUM] = {0};

DIAG_CMD_DRX_REG_WR_IND_STRU *g_pstDataSampleIndVal =  NULL;
DIAG_CMD_DRX_REG_WR_IND_STRU *g_pstLogSampleIndVal[BBP_LOG_SAMPLE_MAX_CHAN_NUM] = {0};

VOS_BOOL g_blDataSampleIsOver = FALSE;

HTIMER g_DataSampleTimer = VOS_NULL_PTR;
HTIMER g_LogSampleTimer = VOS_NULL_PTR;
#if 0
VOS_BOOL LSLEEP_RttIsSleep(VOS_VOID)
{
    return FALSE;
}
#endif
VOS_UINT32 bbp_RegRead(const VOS_VOID* pReadAdd, VOS_VOID* pOutBuff)
{

#if (VOS_OS_VER == VOS_WIN32)
	return ERR_MSP_SUCCESS;
#else
	return (VOS_UINT32)diag_RegRead((VOS_UINT32)pReadAdd, ADDRTYPE32BIT, (VOS_UINT32*)pOutBuff);
#endif
}

VOS_UINT32 bbp_RegWrite(const VOS_VOID* pInBuff, VOS_VOID* pWriteAdd)
{
#if (VOS_OS_VER == VOS_WIN32)
    return ERR_MSP_SUCCESS;
#else
    /*4.1最新确认，HSO下发完整的寄存器地址，MSP直接使用*/
    VOS_UINT32 ulAddr = (VOS_UINT32)pWriteAdd;
    return (VOS_UINT32)diag_RegWrite(ulAddr,ADDRTYPE32BIT,*((VOS_UINT32 *)pInBuff));
#endif
}
VOS_UINT32 bbp_ReportDataSampleInd(ENUM_BBP_REPORT_TYPE_U32 enBbpReportFlag)
{
    VOS_UINT32 ulRet = ERR_MSP_INVALID_PARAMETER;
    DIAG_CMD_DRX_REG_WR_IND_STRU * pIndMessage = NULL;
    VOS_UINT32 ulIndBufLen = 0;

    if(BBP_REPORT_FROM_LAST_BBP_OFF == enBbpReportFlag)
    {
        if(NULL != g_pstDataSampleIndVal)
        {
            ulRet = DIAG_ReportCommand( DIAG_CMD_DRX_REG_WR_SAMPLE_IND,
                    (g_pstDataSampleIndVal->ulTotalRegCount*sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU))+4,
                     g_pstDataSampleIndVal);

            VOS_MemFree(MSP_PID_BBP_AGENT,g_pstDataSampleIndVal);
            g_pstDataSampleIndVal = NULL;
        }
    }
    else
    {
        if(NULL != g_pDataSampleRecordBuf)
        {
            ulIndBufLen = sizeof(DIAG_CMD_DRX_REG_WR_IND_STRU)+(g_pDataSampleRecordBuf->ulTotalRegCount - \
            g_pDataSampleRecordBuf->ulSampleStatusRegIndex) * sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU);

            pIndMessage = (DIAG_CMD_DRX_REG_WR_IND_STRU*)VOS_MemAlloc(MSP_PID_BBP_AGENT, DYNAMIC_MEM_PT, ulIndBufLen);
            if(NULL == pIndMessage)
            {
                return ERR_MSP_FAILURE;
            }

            pIndMessage->ulTotalRegCount = g_pDataSampleRecordBuf->ulTotalRegCount;
            bbp_GetDataSampleIndValue(pIndMessage->stDrxRegPara,g_pDataSampleRecordBuf);

            ulRet = DIAG_ReportCommand( DIAG_CMD_DRX_REG_WR_SAMPLE_IND,
    		                            (pIndMessage->ulTotalRegCount*sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU))+4,
    		                            pIndMessage);

            VOS_MemFree(MSP_PID_BBP_AGENT,pIndMessage);
            pIndMessage = NULL;

        }
    }
	return ulRet;

}


/*0x10|0010|01 --->0x10(end=6)|0010(reg_value =2)|(begin=2)01*/
VOS_BOOL bbp_DataSampleIsOver(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*psDataSample)
{
    VOS_UINT32 ulCheckIndex = 0;
    VOS_UINT32 ulRegRdAddr = 0;
    VOS_UINT16 ucBeginBit = 0;
	VOS_UINT16 ucEndBit = 0;
    VOS_UINT32 ulRdRegValue = 0;

    ulCheckIndex = psDataSample->ulCheckSampleFinishRegIndex;
    ulRegRdAddr = psDataSample->stDrxRegPara[ulCheckIndex].ulAddr;

    bbp_RegRead((VOS_VOID*)ulRegRdAddr,(VOS_VOID*)(&ulRdRegValue));
    ucBeginBit = psDataSample->stDrxRegPara[ulCheckIndex].ucBeginBit;
    ucEndBit = psDataSample->stDrxRegPara[ulCheckIndex].ucEndBit;

    ulRdRegValue = ulRdRegValue<<(31-ucEndBit);
    ulRdRegValue = ulRdRegValue>>(31-ucEndBit+ucBeginBit);

    if(ulRdRegValue == psDataSample->stDrxRegPara[ulCheckIndex].ulRegValue)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
VOS_VOID bbp_GetDataSampleIndValue(DIAG_CMD_DRX_REG_WR_PARA_STRU * pIndRegVal,DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*psDataSample)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulRegRdAddr = 0;
    VOS_UINT32 ulRdRegValue = 0;

    for(i = psDataSample->ulSampleStatusRegIndex;
	    i < psDataSample->ulTotalRegCount;i++)
    {
        VOS_MemCpy(pIndRegVal,&psDataSample->stDrxRegPara[i],sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU));

        ulRegRdAddr = psDataSample->stDrxRegPara[i].ulAddr;
        bbp_RegRead((VOS_VOID*)ulRegRdAddr,(VOS_VOID*)(&ulRdRegValue));
        pIndRegVal->ulRegValue = ulRdRegValue;
		pIndRegVal++;
    }

    return ;
}


VOS_UINT32 bbp_SetDataSampleStart(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*psDataSample)
{
    VOS_UINT32 i = 0,j=0;
    VOS_UINT32 ulRet = 0;
    DIAG_CMD_REG_WR_PARA_STRU *pWrtPara = NULL;

    for( i = psDataSample->ulStartSampleRegIndex;
	     i < psDataSample->ulStopSampleRegIndex; i++ )
    {
        pWrtPara = (DIAG_CMD_REG_WR_PARA_STRU *)(&psDataSample->stDrxRegPara[i]);

        ulRet = diag_SetRegValue(pWrtPara);
        for(j=0;j<psDataSample->stDrxRegPara[i].ulSleepTime;j++)
        {
        	/*for delay*/
        }
    }
    return ulRet;
}
VOS_UINT32 bbp_SetDataSampleOver(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*psDataSample)
{
    VOS_UINT32 i = 0,j=0;
    VOS_UINT32 ulRet = 0;
    DIAG_CMD_REG_WR_PARA_STRU *pWrtPara = NULL;

    /*设置寄存其结束数采*/
	for( i = psDataSample->ulStopSampleRegIndex;
		 i < psDataSample->ulCheckSampleFinishRegIndex;i++)
	{
		pWrtPara=(DIAG_CMD_REG_WR_PARA_STRU *)(&psDataSample->stDrxRegPara[i]);
		ulRet = diag_SetRegValue(pWrtPara);
        for(j=0;j<psDataSample->stDrxRegPara[i].ulSleepTime;j++)
        {
        	/*for delay*/
        }
	}
    return ulRet;
}


VOS_UINT32 bbp_StartDataSampleCallBack( VOS_VOID )
{
	VOS_UINT32 u32Ret = ERR_MSP_FAILURE;

    /*如果数采已经结束，则停止数采*/
    if(g_blDataSampleIsOver == TRUE)
    {
       return ERR_MSP_SUCCESS;
    }

    /*根据是否有下发命令记录，决定是否启动数采*/
    if((NULL != g_pDataSampleRecordBuf)&&(NULL != g_pstDataSampleIndVal))
    {
       /*配置BBP启动数采*/
       u32Ret = bbp_SetDataSampleStart((DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*)g_pDataSampleRecordBuf);
	   if(ERR_MSP_SUCCESS != u32Ret)
	   {
			return u32Ret;
	   }

       /*启动定时器，查询BBP数采进度*/
        /* coverity[check_return] */
       VOS_StartRelTimer(&g_DataSampleTimer, MSP_PID_BBP_AGENT, BBP_DATA_SAMPLE_TIMEROUT_VALUE, 0, \
                        DIAG_TMR_DATA_SAMPLE_TIMEOUT_PARAM, VOS_RELTIMER_NOLOOP,VOS_TIMER_NO_PRECISION);
       return ERR_MSP_SUCCESS;
    }

    return ERR_MSP_FAILURE;

}
VOS_UINT32 bbp_StopDataSampleCallBack( VOS_VOID )
{
	VOS_UINT32 ret = ERR_MSP_SUCCESS;

    /*结束定时器*/
    if(VOS_NULL_PTR != g_DataSampleTimer)
	{
	   VOS_StopRelTimer(&g_DataSampleTimer);
	}
    printf("xxxx\n");
    /*若全局变量为空，说明之前已结束数采*/
    if((NULL == g_pDataSampleRecordBuf)||(NULL == g_pstDataSampleIndVal))
    {
        return ERR_MSP_SUCCESS;
    }
    printf("yyyyy\n");

    /*记录BBP状态数据*/
	bbp_GetDataSampleIndValue(g_pstDataSampleIndVal->stDrxRegPara,g_pDataSampleRecordBuf);

	/*查询BBP寄存器数采进度，如果查询结果数采结束，调用接口上报HSO*/
    if(bbp_DataSampleIsOver(g_pDataSampleRecordBuf))
    {
        /*释放信号量上报数采状态给HSO*/
        VOS_SmV(g_ulDataSampleIndSem);
    }
    else
    {
        /*如果数采未结束，则配置BBP停止数采，记录BBP状态数据*/
        ret = bbp_SetDataSampleOver(g_pDataSampleRecordBuf);
        if(ERR_MSP_SUCCESS != ret)
        {
            return ret;
        }

        /*判断是否是单次数采，如是，则结束数采，调用接口上报HSO；*/
		if(BBP_SAMPLE_COUNTER_MODE == g_pDataSampleRecordBuf->enSampleType)
        {
            g_blDataSampleIsOver = TRUE;
            VOS_MemFree(MSP_PID_BBP_AGENT, g_pDataSampleRecordBuf);
            g_pDataSampleRecordBuf = NULL;
            VOS_SmV(g_ulDataSampleIndSem);
        }
		else
		{
             /*否则SWTICH数采模式，保存BBP状态信息，等待下次DRX周期数采启动*/
		}

    }
    return ERR_MSP_SUCCESS;
}



VOS_UINT32 bbp_DrxDataSampleStartEntry(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU *pstDataSample)
{
    VOS_UINT32 ulRecordBufLen = 0,ulIndBufLen = 0;

	/*如果是启动数采则保存数采数据*/
    ulRecordBufLen = sizeof(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU) + pstDataSample->ulTotalRegCount \
                * sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU);

	g_pDataSampleRecordBuf = (DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*)VOS_MemAlloc(MSP_PID_BBP_AGENT, DYNAMIC_MEM_PT, ulRecordBufLen);
    if(NULL == g_pDataSampleRecordBuf)
    {
        return ERR_MSP_MALLOC_FAILUE;
    }
	VOS_MemCpy(g_pDataSampleRecordBuf, pstDataSample,ulRecordBufLen);

    /*保存给HSO上报的寄存器状态数据*/
    ulIndBufLen = sizeof(DIAG_CMD_DRX_REG_WR_IND_STRU) + (pstDataSample->ulTotalRegCount - pstDataSample->ulSampleStatusRegIndex) \
                * sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU);
    g_pstDataSampleIndVal = (DIAG_CMD_DRX_REG_WR_IND_STRU*)VOS_MemAlloc(MSP_PID_BBP_AGENT, DYNAMIC_MEM_PT, ulIndBufLen);
    if(NULL == g_pstDataSampleIndVal)
    {
        return ERR_MSP_MALLOC_FAILUE;
    }
    g_pstDataSampleIndVal->ulTotalRegCount = g_pDataSampleRecordBuf->ulTotalRegCount;
    return ERR_MSP_SUCCESS;
}


VOS_UINT32 bbp_DrxDataSampleStopEntry(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU *pstDataSample)
{
    VOS_UINT32 ulRet =0;

	/*停止定时器*/
	VOS_StopRelTimer(&g_DataSampleTimer);

    if( LSLEEP_RttIsSleep() == VOS_FALSE )
    {
        /*查询数采是否结束，如未结束，配置BBP寄存器结束数采*/
        if(bbp_DataSampleIsOver(pstDataSample))
        {
            bbp_SetDataSampleOver(pstDataSample);

            /*获取BBP数采状态,调用接口上报HSO*/
            ulRet = bbp_ReportDataSampleInd(BBP_REPORT_FROM_BBP_ON);

        }
    }
    else
    {
        /*查询数采是否结束，如未结束，则结束数采，不能配置寄存器
                    将上次BBP OFF记录的BBP记录信息上报HSO；否则不处理*/
       if(TRUE != g_blDataSampleIsOver)
       {
           ulRet = bbp_ReportDataSampleInd(BBP_REPORT_FROM_LAST_BBP_OFF);
       }

    }

    /*设置数采结束标识，释放申请空间*/
    g_blDataSampleIsOver = TRUE;
    VOS_MemFree(MSP_PID_BBP_AGENT, g_pDataSampleRecordBuf);
    g_pDataSampleRecordBuf = NULL;
    return ulRet;
}
VOS_UINT32 bbp_DataSampleTimerOutProc(VOS_VOID)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;

     /*当前是否BBP下电状态，如不是, 查询BBP寄存器数采进度，否则不查询*/
    if( LSLEEP_RttIsSleep() == VOS_FALSE )
    {
        /*数采结束，调用接口上报HSO*/
        if(NULL != g_pDataSampleRecordBuf)
        {
            if(bbp_DataSampleIsOver(g_pDataSampleRecordBuf))
            {
                /*获取BBP状态，调用接口上报HSO*/
                ulRet = bbp_ReportDataSampleInd(BBP_REPORT_FROM_BBP_ON);
            }
            else
            {
                /*上电未结束需启动定时器，查询BBP数采进度*/
                /* coverity[check_return] */
                VOS_StartRelTimer(&g_DataSampleTimer, MSP_PID_BBP_AGENT, BBP_DATA_SAMPLE_TIMEROUT_VALUE, 0, \
                        DIAG_TMR_DATA_SAMPLE_TIMEOUT_PARAM, VOS_RELTIMER_NOLOOP,VOS_TIMER_NO_PRECISION);
            }

        }
    }
    return ulRet;
}

VOS_UINT32 bbp_DrxLogSampleStartEntry(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU *pstLogSample)
{
    VOS_UINT32 ulRecordBufLen = 0,ulIndBufLen = 0;

	/*如果是启动数采则保存数采数据*/
    /*如果该通道没有申请过，申请空间*/
    if(NULL == g_pLogSampleRecordBuf[pstLogSample->ulLogChannelIndex])
    {
        ulRecordBufLen = sizeof(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU) + pstLogSample->ulTotalRegCount \
                * sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU);

        g_pLogSampleRecordBuf[pstLogSample->ulLogChannelIndex] = (DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*)VOS_MemAlloc(MSP_PID_BBP_AGENT, DYNAMIC_MEM_PT, ulRecordBufLen);
        if(NULL == g_pLogSampleRecordBuf[pstLogSample->ulLogChannelIndex])
        {
            return ERR_MSP_MALLOC_FAILUE;
        }

        VOS_MemCpy(g_pLogSampleRecordBuf[pstLogSample->ulLogChannelIndex], pstLogSample,ulRecordBufLen);
    }


    /*保存给HSO上报的寄存器状态数据*/
    if(NULL == g_pstLogSampleIndVal[pstLogSample->ulLogChannelIndex])
    {
        ulIndBufLen = sizeof(DIAG_CMD_DRX_REG_WR_IND_STRU) + (pstLogSample->ulTotalRegCount - pstLogSample->ulSampleStatusRegIndex) \
                    * sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU);
        g_pstLogSampleIndVal[pstLogSample->ulLogChannelIndex] = (DIAG_CMD_DRX_REG_WR_IND_STRU*)VOS_MemAlloc(MSP_PID_BBP_AGENT, DYNAMIC_MEM_PT, ulIndBufLen);
        if(NULL == g_pstLogSampleIndVal[pstLogSample->ulLogChannelIndex])
        {
            return ERR_MSP_MALLOC_FAILUE;
        }
        g_pstLogSampleIndVal[pstLogSample->ulLogChannelIndex]->ulTotalRegCount = g_pLogSampleRecordBuf[pstLogSample->ulLogChannelIndex]->ulTotalRegCount;
    }
    return ERR_MSP_SUCCESS;
}

VOS_UINT32 bbp_DrxLogSampleStopEntry(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU *pstLogSample)
{
    VOS_UINT32 ulRet =0;

    if( LSLEEP_RttIsSleep() == VOS_FALSE )
    {
        /*配置BBP寄存器结束数采*/
        bbp_SetLogSampleOver(pstLogSample);

        /*获取BBP数采状态,调用接口上报HSO*/
        ulRet = bbp_ReportLogSampleInd(BBP_REPORT_FROM_BBP_ON,pstLogSample->ulLogChannelIndex);

    }
    else
    {
        /*不能配置寄存器,将上次BBP OFF记录的BBP记录信息上报HSO*/
       ulRet = bbp_ReportLogSampleInd(BBP_REPORT_FROM_LAST_BBP_OFF,pstLogSample->ulLogChannelIndex);
    }

    /*设置数采结束标识，释放申请空间*/
    VOS_MemFree(MSP_PID_BBP_AGENT, g_pLogSampleRecordBuf[pstLogSample->ulLogChannelIndex]);
    VOS_MemFree(MSP_PID_BBP_AGENT, g_pstLogSampleIndVal[pstLogSample->ulLogChannelIndex]);
    g_pLogSampleRecordBuf[pstLogSample->ulLogChannelIndex] = NULL;
    g_pstLogSampleIndVal[pstLogSample->ulLogChannelIndex] = NULL;
    return ulRet;
}



VOS_VOID bbp_GetLogSampleIndValue(DIAG_CMD_DRX_REG_WR_PARA_STRU * pIndRegVal,DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*pstLogSample)
{
    VOS_UINT32 i = 0;
    VOS_UINT32 ulRegRdAddr = 0;
    VOS_UINT32 ulRdRegValue = 0;

    for(i = pstLogSample->ulSampleStatusRegIndex;
	    i < pstLogSample->ulTotalRegCount;i++)
    {
        VOS_MemCpy(pIndRegVal,&pstLogSample->stDrxRegPara[i],sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU));

        ulRegRdAddr = pstLogSample->stDrxRegPara[i].ulAddr;
        bbp_RegRead((VOS_VOID*)ulRegRdAddr,(VOS_VOID*)(&ulRdRegValue));

        if(i == pstLogSample->ulAccumlateRegIndex)
        {
            pIndRegVal->ulRegValue += ulRdRegValue;
        }
        else
        {
            pIndRegVal->ulRegValue = ulRdRegValue;
        }
		pIndRegVal++;
    }

    return ;
}

VOS_UINT32 bbp_ReportLogSampleInd(ENUM_BBP_REPORT_TYPE_U32 enBbpReportFlag,VOS_UINT32 ulChanIndex)
{
    VOS_UINT32 ulRet = ERR_MSP_INVALID_PARAMETER;
    DIAG_CMD_DRX_REG_WR_IND_STRU * pIndMessage = NULL;
    VOS_UINT32 ulIndBufLen = 0;

    if(BBP_REPORT_FROM_LAST_BBP_OFF == enBbpReportFlag)
    {
        if(NULL != g_pstLogSampleIndVal[ulChanIndex])
        {
            ulRet = DIAG_ReportCommand( DIAG_CMD_DRX_REG_WR_SAMPLE_IND,
                    (g_pstLogSampleIndVal[ulChanIndex]->ulTotalRegCount*sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU))+4,
                     g_pstLogSampleIndVal[ulChanIndex]);
        }
    }
    else
    {
        if(NULL != g_pLogSampleRecordBuf[ulChanIndex])
        {
            ulIndBufLen = sizeof(DIAG_CMD_DRX_REG_WR_IND_STRU)+(g_pLogSampleRecordBuf[ulChanIndex]->ulTotalRegCount - \
            g_pLogSampleRecordBuf[ulChanIndex]->ulSampleStatusRegIndex) * sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU);

            pIndMessage = (DIAG_CMD_DRX_REG_WR_IND_STRU*)VOS_MemAlloc(MSP_PID_BBP_AGENT, DYNAMIC_MEM_PT, ulIndBufLen);
            if(NULL == pIndMessage)
            {
                return ERR_MSP_FAILURE;
            }

            pIndMessage->ulTotalRegCount = g_pLogSampleRecordBuf[ulChanIndex]->ulTotalRegCount;
            bbp_GetLogSampleIndValue(pIndMessage->stDrxRegPara,g_pLogSampleRecordBuf[ulChanIndex]);


            ulRet = DIAG_ReportCommand( DIAG_CMD_DRX_REG_WR_SAMPLE_IND,
    		                            (pIndMessage->ulTotalRegCount*sizeof(DIAG_CMD_DRX_REG_WR_PARA_STRU))+4,
    		                            pIndMessage);

            VOS_MemFree(MSP_PID_BBP_AGENT,pIndMessage);
            pIndMessage = NULL;

        }
    }
	return ulRet;

}

VOS_UINT32 bbp_SetLogSampleStart(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*pstLogSample)
{
    VOS_UINT32 i = 0,j=0;
    VOS_UINT32 ulRet = 0;
    DIAG_CMD_REG_WR_PARA_STRU *pWrtPara = NULL;

    for( i = pstLogSample->ulLogStartRegIndex;
	     i < pstLogSample->ulLogStopRegIndex; i++ )
    {
        pWrtPara = (DIAG_CMD_REG_WR_PARA_STRU *)(&pstLogSample->stDrxRegPara[i]);

        ulRet = diag_SetRegValue(pWrtPara);

        for(j=0;j<pstLogSample->stDrxRegPara[i].ulSleepTime;j++)
        {
        	/*for delay*/
        }
    }
    return ulRet;
}
VOS_UINT32 bbp_SetLogSampleOver(DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*pstLogSample)
{
    VOS_UINT32 i = 0,j=0;
    VOS_UINT32 ulRet = 0;
    DIAG_CMD_REG_WR_PARA_STRU *pWrtPara = NULL;

    /*设置寄存其结束数采*/
	for( i = pstLogSample->ulLogStopRegIndex;
		 i < pstLogSample->ulSampleStatusRegIndex;i++)
	{
		pWrtPara=(DIAG_CMD_REG_WR_PARA_STRU *)(&pstLogSample->stDrxRegPara[i]);
		ulRet = diag_SetRegValue(pWrtPara);
        for(j=0;j<pstLogSample->stDrxRegPara[i].ulSleepTime;j++)
        {
        	/*for delay*/
        }
	}
    return ulRet;
}

VOS_UINT32 bbp_StartLogSampleCallBack( VOS_VOID )
{
    VOS_UINT32 ulChanIndex =0;
    VOS_UINT32 ulInvalidChan =0;

    /*根据是否有下发命令记录，决定是否启动数采*/
    for(ulChanIndex = 0 ;ulChanIndex< BBP_LOG_SAMPLE_MAX_CHAN_NUM ;ulChanIndex++)
    {
        if((NULL != g_pLogSampleRecordBuf[ulChanIndex])&&(NULL != g_pstLogSampleIndVal[ulChanIndex]))
        {
           /*配置BBP启动数采*/
           bbp_SetLogSampleStart((DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*)g_pLogSampleRecordBuf[ulChanIndex]);
           ulInvalidChan++;
        }
    }

   /*如果有通道启动数采，启动定时器，上报BBP数采进度*/
   if(ulInvalidChan > 0)
   {
    /* coverity[check_return] */
       VOS_StartRelTimer(&g_LogSampleTimer, MSP_PID_BBP_AGENT, BBP_LOG_SAMPLE_TIMEOUT_VALUE, 0, \
                    DIAG_TMR_LOG_SAMPLE_TIMEOUT_PARAM, VOS_RELTIMER_NOLOOP,VOS_TIMER_NO_PRECISION);
   }

    return ERR_MSP_SUCCESS;

}


VOS_UINT32 bbp_StopLogSampleCallBack( VOS_VOID )
{
    VOS_UINT32 ulChanIndex = 0;

    /*下电时，结束定时器，关闭定时上报*/
    VOS_StopRelTimer(&g_LogSampleTimer);

    /*记录BBP状态数据*/
    for(ulChanIndex = 0 ;ulChanIndex< BBP_LOG_SAMPLE_MAX_CHAN_NUM ;ulChanIndex++)
    {
        if((NULL != g_pLogSampleRecordBuf[ulChanIndex])&&(NULL != g_pstLogSampleIndVal[ulChanIndex]))
        {
           bbp_GetDataSampleIndValue(((g_pstLogSampleIndVal[ulChanIndex]->stDrxRegPara)),(DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*)(g_pLogSampleRecordBuf[ulChanIndex]));
        }
    }
	/*释放信号量上报数采状态给HSO*/
    VOS_SmV(g_ulDataSampleIndSem);

    return ERR_MSP_SUCCESS;
}

VOS_UINT32 bbp_LogSampleTimerOutProc(VOS_VOID)
{
    VOS_UINT32 ulRet = ERR_MSP_SUCCESS;
    VOS_UINT32 ulChanIndex = 0;

     /*当前是否BBP下电状态，如不是, 查询BBP寄存器数采进度，否则不查询*/
    if( LSLEEP_RttIsSleep() == VOS_FALSE )
    {
        /*获取BBP状态，调用接口上报HSO*/
         /* coverity[check_return] */
        for(ulChanIndex = 0;ulChanIndex < BBP_LOG_SAMPLE_MAX_CHAN_NUM ;ulChanIndex++)
        {
            ulRet = bbp_ReportLogSampleInd(BBP_REPORT_FROM_BBP_ON,ulChanIndex);
        }
         /*上电未结束需启动定时器，查询BBP数采进度*/
        VOS_StartRelTimer(&g_LogSampleTimer, MSP_PID_BBP_AGENT, BBP_LOG_SAMPLE_TIMEOUT_VALUE, 0, \
                  DIAG_TMR_LOG_SAMPLE_TIMEOUT_PARAM, VOS_RELTIMER_NOLOOP,VOS_TIMER_NO_PRECISION);
    }
    return ulRet;
}
VOS_VOID bbp_AgentSetSleepFlag(VOS_BOOL blAgentSleepFlag)
{
    g_blBbpAgentSleepFlag = blAgentSleepFlag;
}


VOS_BOOL bbp_AgentGetSleepFlag(VOS_VOID)
{
    return g_blBbpAgentSleepFlag;
}


VOS_UINT32 bbp_DataLogInitGlobal( VOS_VOID )
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;
	ret = VOS_SmBCreate( "BBP_DATA_SAMPLE", 0, VOS_SEMA4_FIFO, (VOS_UINT32*)&g_ulDataSampleIndSem);
    return ret;
}


VOS_VOID bbp_DataLogIndSelfTask(VOS_VOID)
{
	VOS_UINT32 ulChanIndex = 0;

    /* coverity[loop_bottom] */
    /* coverity[no_escape] */
    for (;;)
    {
        (VOS_VOID)VOS_SmP(g_ulDataSampleIndSem,0);

        /*上报数采状态*/
        bbp_ReportDataSampleInd(BBP_REPORT_FROM_LAST_BBP_OFF);

        /*上报LOG 数采状态*/
		for( ulChanIndex = 0; ulChanIndex < BBP_LOG_SAMPLE_MAX_CHAN_NUM; ulChanIndex++ )
		{
            bbp_ReportLogSampleInd(BBP_REPORT_FROM_LAST_BBP_OFF,ulChanIndex);
		}
    /* coverity[loop_bottom] */
    }

}
VOS_UINT32 bbp_AgentMsgProcInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    VOS_UINT32 ret = ERR_MSP_SUCCESS;

    if (ip == VOS_IP_RESTART)
    {
       ret = bbp_DataLogInitGlobal();
    }
    return ret;
}


VOS_VOID bbp_AgentTimeOutProcEntry(VOS_VOID* pstPara)
{

    /*超时消息*/
    if( DIAG_TMR_DATA_SAMPLE_TIMEOUT_PARAM == (int)pstPara)
    {
        bbp_DataSampleTimerOutProc();
    }
    else if( DIAG_TMR_LOG_SAMPLE_TIMEOUT_PARAM == (int)pstPara)
    {
    	bbp_LogSampleTimerOutProc();
    }

    return ;
}
VOS_VOID bbp_AgentMsgProc(MsgBlock* pMsgBlock)
{
    DIAG_DATA_MSG_STRU* pMsgTmp =NULL;
    REL_TIMER_MSG *pTimer =NULL;

     /*入参判断*/
    if (NULL == pMsgBlock)
    {
        return;
    }

    /*任务开始处理，不允许睡眠*/
    bbp_AgentSetSleepFlag(FALSE);

    switch(pMsgBlock->ulSenderPid)
    {
        case DOPRA_PID_TIMER:
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_BBP_AGENT_TIME_OUT_ENTRY,0,0,0);
            pTimer   = (REL_TIMER_MSG*)pMsgBlock;
            bbp_AgentTimeOutProcEntry((VOS_VOID*)(pTimer->ulPara));
            break;

        case MSP_PID_DIAG_AGENT:
            DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_BBP_AGENT_TIME_OUT_ENTRY,0,0,1);
            pMsgTmp = (DIAG_DATA_MSG_STRU*)pMsgBlock;

            switch(pMsgTmp->ulMsgId)
            {
               case ID_MSG_REQ_DRX_DATA_SAMPLE_START:
                   bbp_DrxDataSampleStartEntry((DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*)pMsgTmp->pContext);
                   break;
               case ID_MSG_REQ_DRX_DATA_SAMPLE_STOP:
                   bbp_DrxDataSampleStopEntry((DIAG_CMD_DRX_DATA_SAMPLE_REG_WR_REQ_STRU*)pMsgTmp->pContext);
                   break;
               case ID_MSG_REQ_DRX_LOG_SAMPLE_START:
                   bbp_DrxLogSampleStartEntry((DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU *)pMsgTmp->pContext);
                   break;
               case ID_MSG_REQ_DRX_LOG_SAMPLE_STOP:
                   bbp_DrxLogSampleStopEntry((DIAG_CMD_DRX_LOG_SAMPLE_REG_WR_REQ_STRU*)pMsgTmp->pContext);
                   break;
               default:
                   break;
            }
            break;
        default:
            break;

    }
    /*任务开始结束，允许睡眠*/
    bbp_AgentSetSleepFlag(TRUE);
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



