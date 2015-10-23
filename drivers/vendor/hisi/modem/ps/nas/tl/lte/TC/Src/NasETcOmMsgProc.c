/******************************************************************************

   Copyright(C)2008,Hisilicon Co. LTD.

 ******************************************************************************
  File Name       : NasTcOmMsgProc.c
  Description     :
  History         :
     1.lihong       2010-04-15   Draft Enact

******************************************************************************/


/*****************************************************************************
  1 Include HeadFile
*****************************************************************************/
#include "NasETcOmMsgProc.h"
#include "NasETcPublic.h"

/*lint -e767*/
#define    THIS_FILE_ID        PS_FILE_ID_NASTCOMMSGPROC_C
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


/*****************************************************************************
  3 Function
*****************************************************************************/
/*****************************************************************************
 Function Name   : NAS_ETC_SndAirMsgReportInd
 Description     : 发送空口消息
 Input           : None
 Output          : None
 Return          : VOS_VOID

 History         :
    1.lihong00150010      2010-04-15  Draft Enact

*****************************************************************************/
/*lint -e960*/
/*lint -e961*/
VOS_VOID NAS_ETC_SndAirMsgReportInd
(
    const VOS_UINT8                    *pucData,
    VOS_UINT32                          ulLength,
    NAS_ETC_AIR_MSG_DIR_ENUM_UINT8       enMsgDir,
    OM_PS_AIR_MSG_ENUM_UINT8            enMsgId
)
{
#ifndef PS_ITT_PC_TEST
    DIAG_AIR_MSG_LOG_STRU       stAirMsg;
    VOS_UINT8                  *pucTmpData = VOS_NULL_PTR;

    /*分配空口消息空间*/
    pucTmpData = (VOS_UINT8*)
                        NAS_ETC_MEM_ALLOC(ulLength);

    if(VOS_NULL_PTR== pucTmpData)
    {
        NAS_ETC_ERR_LOG("NAS_ETC_SndAirMsgReportInd: MEM_ALLOC ERR!");
        return;
    }

    NAS_ETC_MEM_CPY(pucTmpData, pucData, ulLength);

    /*设置空口消息方向*/
    if(NAS_ETC_AIR_MSG_DIR_ENUM_UP == enMsgDir)
    {
        stAirMsg.ulId = DIAG_AIR_MSG_LOG_ID(PS_PID_TC, OS_MSG_UL);
        stAirMsg.ulSideId = DIAG_SIDE_NET;
    }
    else
    {
        stAirMsg.ulId = DIAG_AIR_MSG_LOG_ID(PS_PID_TC, OS_MSG_DL);
        stAirMsg.ulSideId = DIAG_SIDE_UE;
    }
    stAirMsg.ulMessageID = (VOS_UINT32)(enMsgId+ PS_MSG_ID_AIR_BASE);
    stAirMsg.ulDestMod = 0;
    stAirMsg.ulDataSize = ulLength;
    stAirMsg.pData = (void*)pucTmpData;

    /*上报空口消息*/
    if(ERR_MSP_SUCCESS != DIAG_ReportAirMessageLog(&stAirMsg))
    {
        NAS_ETC_WARN_LOG("NAS_ETC_SndAirMsgReportInd: Send Msg Fail!");
    }

    /*释放空口消息空间*/
    NAS_ETC_MEM_FREE(pucTmpData);
#endif

    NAS_ETC_NORM_LOG("TC->OMT: PS_OMT_AIR_MSG_REPORT_IND\r");
}

/*****************************************************************************
 Function Name  : NAS_ETC_SndKeyEventReportInd()
 Description    : 作成和发送OM_PS_KEY_EVENT_REPORT_IND消息
 Input          : VOS_UINT8 ucKeyEvent 关键事件
 Output         : VOS_VOID
 Return Value   : VOS_VOID

 History        :
    1.lihong00150010      2010-04-15  Draft Enact

*****************************************************************************/
VOS_VOID NAS_ETC_SndKeyEventReportInd(OM_PS_KEY_EVENT_ENUM_UINT8 enKeyEvent)
{
#ifndef PS_ITT_PC_TEST
    /*lint -e778*/
    /*lint -e648*/
    if(ERR_MSP_SUCCESS != DIAG_ReportEventLog(MODID(UE_MODULE_TC_ID, LOG_TYPE_INFO),\
                            (VOS_UINT32)enKeyEvent+OM_PS_KEY_EVENT_BASE_ID))
    {
        NAS_ETC_WARN_LOG("NAS_ETC_SndKeyEventReportInd: Send Msg Fail!");
    }
    /*lint +e648*/
    /*lint +e778*/
#endif

    NAS_ETC_NORM_LOG("TC->OMT: PS_OMT_KEY_EVT_REPORT_IND\r");
}
/*lint +e961*/
/*lint +e960*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

