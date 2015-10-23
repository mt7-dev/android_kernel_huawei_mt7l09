/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : AgingTest.c
  版 本 号   : 初稿
  作    者   : 甘兰 47350
  生成日期   : 2009年10月14日
  最近修改   :
  功能描述   : 老化测试
  函数列表   :
  修改历史   :
  1.日    期   : 2009年10月14日
    作    者   : 甘兰 47350
    修改内容   : 创建文件

******************************************************************************/


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "agingtest.h"
#include "NVIM_Interface.h"
#include "pslog.h"
#include "phyoaminterface.h"
#include "apminterface.h"
#include "omnvinterface.h"
#include "NasNvInterface.h"


#define    THIS_FILE_ID        PS_FILE_ID_AGING_TEST_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

/*保存老化测试的NV项内容*/
OM_AGING_TEST_NV_STRU          *g_pstOmAgingTestNV = VOS_NULL_PTR;
/*保存老化测试当前测试项的下标和网络模式*/
OM_AGING_TEST_CTRL_STRU         g_stOmAgingTestCtrl = {0, MODEM_ID_BUTT, VOS_RATMODE_BUTT};

/*保存老化测试中发送给G物理层配置消息包的指针*/
GPHY_OAM_RF_AGING_TEST_REQ_STRU *g_apstGAgingTestMsg[MODEM_ID_BUTT];
/*保存老化测试中发送给W物理层配置消息包的指针*/
WPHY_OAM_RF_AGING_TEST_REQ_STRU *g_pstWAgingTestMsg = VOS_NULL_PTR;


/*老化测试定时器*/
HTIMER                          g_hAgingTestTimer = VOS_NULL_PTR;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID Aging_TestProc(VOS_VOID)
{
    MODEM_ID_ENUM_UINT16                enModemID;
    VOS_UINT16                          usBand;
    VOS_UINT16                          usChannel;
    VOS_UINT16                          usNetworkMode;
    VOS_UINT16                          usPower;
    VOS_UINT16                          usBandIndex;
    VOS_UINT16                          usModType;
    VOS_UINT16                          usSlotCnt;

    /*指示当前的测试项*/
    usBandIndex = g_stOmAgingTestCtrl.usBandIndex;
    /*假如已经到达测试项末尾，则需要从头开始*/
    if ((usBandIndex + 1) == g_pstOmAgingTestNV->ulNumber)
    {
        g_stOmAgingTestCtrl.usBandIndex = 0;
    }
    else
    {
        g_stOmAgingTestCtrl.usBandIndex = usBandIndex + 1;
    }

    enModemID       = g_pstOmAgingTestNV->astOmRfConfig[usBandIndex].enModemID;
    usBand          = g_pstOmAgingTestNV->astOmRfConfig[usBandIndex].usBand;
    usChannel       = g_pstOmAgingTestNV->astOmRfConfig[usBandIndex].usChannel;
    usNetworkMode   = g_pstOmAgingTestNV->astOmRfConfig[usBandIndex].usNetworkMode;
    usModType       = g_pstOmAgingTestNV->astOmRfConfig[usBandIndex].usModType;
    usSlotCnt       = g_pstOmAgingTestNV->astOmRfConfig[usBandIndex].usSlotCnt;

    if ((enModemID >= MODEM_ID_BUTT) || (usNetworkMode > VOS_RATMODE_WCDMA))
    {
        PS_LOG2(WUEPS_PID_AGING, 0, PS_PRINT_ERROR, "Aging_TestProc:Para is error.",
                    (VOS_INT32)enModemID, (VOS_INT32)usNetworkMode);
        return;
    }

    /*如果当前测试项的网络模式不同于上次，则需要重新加载物理层*/
    if ((enModemID != g_stOmAgingTestCtrl.enModemID)
        ||(usNetworkMode != g_stOmAgingTestCtrl.usNetworkMode))
    {
        /*停止上一次的RF发射*/
        if (VOS_RATMODE_GSM == g_stOmAgingTestCtrl.usNetworkMode)
        {
            g_apstGAgingTestMsg[g_stOmAgingTestCtrl.enModemID]->enTxEnable = UPHY_OAM_AGING_TEST_OFF;
            (VOS_VOID)VOS_SendMsg(WUEPS_PID_AGING, g_apstGAgingTestMsg[g_stOmAgingTestCtrl.enModemID]);

            /*需要等待物理层的回复，1s为估计值*/
            VOS_TaskDelay(1000);
        }
        else if (VOS_RATMODE_WCDMA == g_stOmAgingTestCtrl.usNetworkMode)
        {
            g_pstWAgingTestMsg->enTxOnOff = UPHY_OAM_AGING_TEST_OFF;
            (VOS_VOID)VOS_SendMsg(WUEPS_PID_AGING, g_pstWAgingTestMsg);

            /*需要等待物理层的回复，1s为估计值*/
            VOS_TaskDelay(1000);
        }
        else
        {
            ;/*Just make pclint happy*/
        }

        /*加载物理层*/
        if (VOS_TRUE != SHPA_LoadPhy((VOS_RATMODE_ENUM_UINT32)usNetworkMode, enModemID, UPHY_OAM_BUSINESS_TYPE_CT))
        {
            /*停止定时器*/
            OM_STOP_AGING_TEST_TIMER(&g_hAgingTestTimer);
            PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_ERROR, "Aging_TestProc:SHPA_LoadPhy fail!");
            return;
        }

        g_stOmAgingTestCtrl.enModemID     = enModemID;
        g_stOmAgingTestCtrl.usNetworkMode = usNetworkMode;
    }

    usPower = g_pstOmAgingTestNV->astOmRfConfig[usBandIndex].usPower;
    /*根据不同的网络模式进行RF配置*/
    if (VOS_RATMODE_GSM == usNetworkMode)
    {
        g_apstGAgingTestMsg[enModemID]->enTxEnable = UPHY_OAM_AGING_TEST_ON;
        g_apstGAgingTestMsg[enModemID]->usFreqNum  =
                         (VOS_UINT16)(usBand << OM_G_RF_BAND_SHIFT) | usChannel;
        g_apstGAgingTestMsg[enModemID]->usTxPower  = usPower;
        g_apstGAgingTestMsg[enModemID]->usSlotCnt = usSlotCnt;
        g_apstGAgingTestMsg[enModemID]->usModType = usModType;

        (VOS_VOID)VOS_SendMsg(WUEPS_PID_AGING, g_apstGAgingTestMsg[enModemID]);
    }
    else if (VOS_RATMODE_WCDMA == usNetworkMode)
    {
        g_pstWAgingTestMsg->enTxOnOff   = UPHY_OAM_AGING_TEST_ON;
        g_pstWAgingTestMsg->usTxBand    = usBand;
        g_pstWAgingTestMsg->usTxChannel = usChannel;
        g_pstWAgingTestMsg->usTxPower   = usPower;

        (VOS_VOID)VOS_SendMsg(WUEPS_PID_AGING, g_pstWAgingTestMsg);
    }
    else
    {
        PS_LOG1(WUEPS_PID_AGING, 0, PS_PRINT_WARNING,
              "Aging_TestProc:usNetworkMode isn't corrcet.", usNetworkMode);
    }

    return;
}


VOS_VOID Aging_FreeMem(VOS_VOID)
{
    MODEM_ID_ENUM_UINT16                enModemID;

    if (VOS_NULL_PTR != g_pstOmAgingTestNV)
    {
        VOS_MemFree(WUEPS_PID_AGING, g_pstOmAgingTestNV);
    }

    for (enModemID = MODEM_ID_0; enModemID < MODEM_ID_BUTT; enModemID++)
    {
        if (VOS_NULL_PTR != g_apstGAgingTestMsg[enModemID])
        {
            VOS_FreeMsg(WUEPS_PID_AGING, g_apstGAgingTestMsg[enModemID]);
        }
    }

    if (VOS_NULL_PTR != g_pstOmAgingTestNV)
    {
        VOS_FreeMsg(WUEPS_PID_AGING, g_pstWAgingTestMsg);
    }

    return;
}


VOS_VOID Aging_MsgProc(MsgBlock* pMsg)
{
    VOS_UINT32 ulName;

    if (VOS_NULL_PTR == pMsg)
    {
        PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_ERROR, "OM_MsgProc:pMsg is empty.");
        return;
    }

    OM_RecordInfoStart(VOS_EXC_DUMP_MEM_NUM_2, pMsg->ulSenderPid, WUEPS_PID_AGING, *((VOS_UINT32*)pMsg->aucValue));

    if (VOS_PID_TIMER == pMsg->ulSenderPid)
    {
        ulName = ((REL_TIMER_MSG*)pMsg)->ulName;

        if (OM_AGING_START_TIMER == ulName)
        {
            ;
        }

        if ((OM_AGING_START_TIMER == ulName)
            || (OM_AGING_TEST_TIMER == ulName))
        {
            Aging_TestProc();

            /*启动老化测试定时器*/
            if (VOS_OK != OM_START_AGING_TEST_TIMER(&g_hAgingTestTimer,
                                                    g_pstOmAgingTestNV->ulTimerLen))
            {
                Aging_FreeMem();
                PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_ERROR, "OM_TimerMsgProc:Start timer fail!");

                OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_2);

                return;
            }
        }
    }

    OM_RecordInfoEnd(VOS_EXC_DUMP_MEM_NUM_2);

    return;
}
VOS_UINT32 Aging_TestInit(VOS_VOID)
{
    NAS_MMA_NVIM_RF_AUTO_TEST_FLAG_STRU stRfAutoTestFlg;
    MODEM_ID_ENUM_UINT16                enModemID;
    VOS_UINT32                          aulReceiverPid[] = {I0_DSP_PID_GPHY, I1_DSP_PID_GPHY};

    VOS_MemSet(g_apstGAgingTestMsg, VOS_NULL_PTR, sizeof(g_apstGAgingTestMsg));

    g_pstOmAgingTestNV = (OM_AGING_TEST_NV_STRU*)VOS_MemAlloc(
                  WUEPS_PID_AGING, STATIC_MEM_PT, sizeof(OM_AGING_TEST_NV_STRU));

    if (VOS_NULL_PTR == g_pstOmAgingTestNV)
    {
        PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_WARNING, "Aging_TestInit: VOS_MemAlloc fail.\n");
        return VOS_ERR;
    }

    /*读取老化测试需要的NV项*/
    if(NV_OK != NV_Read(en_NV_Item_AGING_TEST_TABLE,
                        g_pstOmAgingTestNV,
                        sizeof(OM_AGING_TEST_NV_STRU)))
    {
        PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_WARNING, "Aging_TestInit: NV_Read fail.\n");
        return VOS_ERR;
    }
    /*判断老化测试是否开启*/
    if (VOS_NO == g_pstOmAgingTestNV->ulIsEnable)
    {
        return VOS_ERR;
    }

    /*读取自动开机使能位*/
    if(NV_OK != NV_Read(en_NV_Item_RF_Auto_Test_Flag,
                        &stRfAutoTestFlg,
                        sizeof(NAS_MMA_NVIM_RF_AUTO_TEST_FLAG_STRU)))
    {
        PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_WARNING, "Aging_TestInit: RF NV_Read fail.\n");
        return VOS_ERR;
    }

    if(VOS_YES == stRfAutoTestFlg.usRfAutoTestFlg)
    {
        PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_WARNING, "Aging_TestInit: Auto test enabled!\n");
        return VOS_ERR;
    }

    /*参数检测*/
    if ((0 == g_pstOmAgingTestNV->ulNumber)
        || (OM_AGING_TEST_NUM < g_pstOmAgingTestNV->ulNumber)
        || (0 == g_pstOmAgingTestNV->ulTimerLen))
    {
        return VOS_ERR;
    }

    PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_INFO, "Aging_TestInit: Aging test is launched.\n");

    DRV_PWRCTRL_SLEEPVOTE_LOCK(PWRCTRL_SLEEP_FTM);

    for (enModemID = MODEM_ID_0; enModemID < MODEM_ID_BUTT; enModemID++)
    {
        /*初始化发送给G物理层的配置数据包*/
        g_apstGAgingTestMsg[enModemID] = (GPHY_OAM_RF_AGING_TEST_REQ_STRU*)VOS_AllocMsg(
                        WUEPS_PID_AGING, sizeof(GPHY_OAM_RF_AGING_TEST_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

        if (VOS_NULL_PTR == g_apstGAgingTestMsg[enModemID])
        {
            PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_WARNING, "Aging_TestInit: VOS_AllocMsg fail.\n");
            return VOS_ERR;
        }

        VOS_ReserveMsg(WUEPS_PID_AGING, (MsgBlock *)g_apstGAgingTestMsg[enModemID]);

        g_apstGAgingTestMsg[enModemID]->ulReceiverPid = aulReceiverPid[enModemID];
        g_apstGAgingTestMsg[enModemID]->enMsgId = ID_OAM_GPHY_RF_AGING_TEST_REQ;
    }

    /*初始化发送给W物理层的配置数据包*/
    g_pstWAgingTestMsg = (WPHY_OAM_RF_AGING_TEST_REQ_STRU*)VOS_AllocMsg(
                    WUEPS_PID_AGING, sizeof(WPHY_OAM_RF_AGING_TEST_REQ_STRU) - VOS_MSG_HEAD_LENGTH);

    if (VOS_NULL_PTR == g_pstWAgingTestMsg)
    {
        PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_WARNING, "Aging_TestInit: VOS_AllocMsg fail.\n");
        return VOS_ERR;
    }

    VOS_ReserveMsg(WUEPS_PID_AGING, (MsgBlock *)g_pstWAgingTestMsg);

    g_pstWAgingTestMsg->ulReceiverPid = DSP_PID_WPHY;
    g_pstWAgingTestMsg->enMsgID = ID_OAM_WPHY_RF_AGING_TEST_REQ;

    /*启动老化测试定时器*/
    if (VOS_OK != VOS_StartRelTimer(&g_hAgingTestTimer,
                      WUEPS_PID_AGING, 10000, OM_AGING_START_TIMER, 0,
                      VOS_RELTIMER_NOLOOP, VOS_TIMER_NO_PRECISION))
    {
        PS_LOG(WUEPS_PID_AGING, 0, PS_PRINT_ERROR, "Aging_TestInit:VOS_StartRelTimer fail!");
        return VOS_ERR;
    }

    return VOS_OK;
}
VOS_UINT32 WuepsAgingPidInit(enum VOS_INIT_PHASE_DEFINE ip)
{
    switch( ip )
    {
        case VOS_IP_LOAD_CONFIG:
            if (VOS_OK != Aging_TestInit())
            {
                Aging_FreeMem();
            }
            break;

        default:
            break;
    }

    return VOS_OK;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

