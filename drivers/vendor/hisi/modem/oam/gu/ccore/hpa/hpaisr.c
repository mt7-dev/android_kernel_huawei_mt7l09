/*****************************************************************************/
/*                                                                           */
/*                Copyright 1999 - 2003, Huawei Tech. Co., Ltd.              */
/*                           ALL RIGHTS RESERVED                             */
/*                                                                           */
/* FileName: HpaIsr.c                                                        */
/*                                                                           */
/* Author: Xu cheng                                                          */
/*                                                                           */
/* Version: 1.0                                                              */
/*                                                                           */
/* Date: 2008-02                                                             */
/*                                                                           */
/* Description: implement HPA Isr subroutine                                 */
/*                                                                           */
/* Others:                                                                   */
/*                                                                           */
/* History:                                                                  */
/* 1. Date: 2008-02                                                          */
/*    Author: Xu cheng                                                       */
/*    Modification: Create this file                                         */
/*                                                                           */
/*****************************************************************************/

#include "hpacomm.h"
#include "bbp_wcdma_on_interface.h"
#include "bbp_wcdma_interface.h"
#include "gbbp_interface.h"
#include "sleepflow.h"
#include "spysystem.h"
#include "apminterface.h"
#include "TtfOamInterface.h"
#include "HifiOmInterface.h"
#include "product_config.h"
#include "omprivate.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


/* Macro of log */
#define    THIS_FILE_ID        PS_FILE_ID_HPA_ISR_C
typedef  void (*VOID_FUNCPTR) ();


/*记录ZSP Watch Dog中断号*/
VOS_INT             g_ulZSPWatchDogIntNO = 0;

/*记录HIFI Watch Dog中断号*/
VOS_INT             g_ulHIFIWatchDogIntNO =0;

/*记录GBBP 帧中断号*/
VOS_INT             g_aulGBBPIntNO[MODEM_ID_BUTT] = {0};

/*记录GBBP 唤醒中断号*/
VOS_INT             g_aulGBBPAwakeIntNO[MODEM_ID_BUTT] = {0};

/*记录SIMI 中断号*/
VOS_INT             g_ulSIMIIntNO = 0;

/*记录WBBP 帧中断号*/
VOS_INT             g_ulWBBP0MsIntNO = 0;

/*记录WBBP 唤醒中断号*/
VOS_INT             g_ulWBBPAwakeIntNO = 0;

/*记录WBBP 时钟切换中断号*/
VOS_INT             g_ulWBBPSwtichIntNO = 0;

VOS_UINT32          g_ul3GISRNum = 0;
VOS_UINT32          g_ul2GISRNum0= 0;
VOS_UINT32          g_ul2GISRNum1= 0;

#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
extern VOS_VOID I1_SLEEP_32KIsr(SLEEP_MODE_ENUM_UINT32 enMode);
extern VOS_VOID I1_SLEEP_NormalClockIsr(SLEEP_MODE_ENUM_UINT32 enMode);
extern VOS_VOID I1_SLEEP_AwakeIsr(SLEEP_MODE_ENUM_UINT32 enMode);
extern VOS_VOID I1_SLEEP_HookMsgProc(MsgBlock *pstMsg);
extern VOS_BOOL I1_SLEEP_IsAwakeProc(SLEEP_MODE_ENUM_UINT32 enMode);
#endif
/*****************************************************************************
 Function   : atomic_inc
 Description: increasde atomic counter.comes from linux
 Input      : atomic counter
 Return     : void
 Other      :
 *****************************************************************************/
void atomic_inc( hpa_atomic_t *v )
{
    int flags;

    flags = VOS_SplIMP();

    (v->counter)++;

    VOS_Splx(flags);

    return;
}

/*****************************************************************************
 Function   : HPA_3GIsrEnable
 Description: 3G Isr Enable
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_3GIsrEnable(MODEM_ID_ENUM_UINT16 enModemID)
{
    if (MODEM_ID_0 == enModemID)
    {
        /*Clear W frmae interrupt*/
        HPA_Write32Reg(WBBP_ARM_INT01_CLR_ADDR, 1);

        /*Enable W frame interrupt*/
        HPA_Write32Reg(WBBP_ARM_INT01_MASK_ADDR, 0xFFFFFFFE);
        HPA_Write32Reg(WBBP_ARM_INT01_EN_ADDR, 1);
        DRV_VICINT_ENABLE(g_ulWBBP0MsIntNO);
    }

    return;
}

/*****************************************************************************
 Function   : HPA_3GIsrDisable
 Description: 3G Isr Disable
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_3GIsrDisable(MODEM_ID_ENUM_UINT16 enModemID)
{
    if (MODEM_ID_0 == enModemID)
    {
        /*Disable W frame INT*/
        DRV_VICINT_DISABLE(g_ulWBBP0MsIntNO);

        /*Clear W frmae interrupt*/
        HPA_Write32Reg(WBBP_ARM_INT01_CLR_ADDR, 1);
    }

    return ;
}

/*****************************************************************************
 Function   : HPA_2GIsrEnable
 Description: 2G Isr Enable
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_2GIsrEnable(MODEM_ID_ENUM_UINT16 enModemID)
{
    if (MODEM_ID_BUTT <= enModemID)
    {
        return;
    }

    /* clear interrupt */
    HPA_Write32Reg(g_aulGBBPRegAddr[INT_CLR][enModemID], 1);

    /*Enable G frame interrupt*/
    HPA_Clear32RegMask(g_aulGBBPRegAddr[INT_MASK][enModemID], OAM_GBBP_FRAME_INT_BIT);

    /*lint -e662 -e661*/
    DRV_VICINT_ENABLE(g_aulGBBPIntNO[enModemID]);
    /*lint +e662 +e661*/

    return;
}

/*****************************************************************************
 Function   : HPA_2GIsrDisable
 Description: 2G Isr Disable
 Input      : MODEM_ID_ENUM_UINT16 enModemID
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_2GIsrDisable(MODEM_ID_ENUM_UINT16 enModemID)
{
    if (MODEM_ID_BUTT <= enModemID)
    {
        return;
    }

    /*Disable W frame INT*/
    /*lint -e662 -e661*/
    DRV_VICINT_DISABLE(g_aulGBBPIntNO[enModemID]);
    /*lint +e662 +e661*/

    /* clear interrupt */
    HPA_Write32Reg(g_aulGBBPRegAddr[INT_CLR][enModemID], 1);

    return ;
}

/*lint -e960 修改人：甘兰47350；检视人：xucheng 51137；原因简述：清中断*/

/*****************************************************************************
 Function   : HPA_2GFrameClockIsr
 Description: Gsm BBP frame and clock switch ISR
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_2GFrameClockIsr(MODEM_ID_ENUM_UINT16 enModemID)
{
    VOS_UINT32  ulBbpStatus;
    VOS_UINT32  ulDrxStatus;
    VOS_UINT32  ulBeginSlice;
    VOS_UINT32  i;

    if(enModemID >= MODEM_ID_BUTT)
    {
        return;
    }

    if (PWRCTRL_COMM_OFF == DRV_PWRCTRL_PWRSTATUSGET((PWC_COMM_MODE_E)VOS_RATMODE_GSM, PWC_COMM_MODULE_BBP_DRX, (PWC_COMM_MODEM_E)enModemID))
    {
        VOS_ProtectionReboot(DRX_REPORT_BBP_POWERDOWN_ERROR, PS_FILE_ID_HPA_ISR_C, __LINE__,
                VOS_NULL_PTR, 0);

        return;
    }

#if defined (CHIP_BB_6620CS)
    if (MODEM_ID_0 == enModemID)
    {
        g_stHpaIntCount.ulSOCStatRecord[0]  = HPA_Read32Reg(0xF711A674);
        g_stHpaIntCount.ulSOCStatRecord[1]  = HPA_Read32Reg(0xF711A6A8);
        g_stHpaIntCount.ulSOCStatRecord[2]  = HPA_Read32Reg(0xF711A710);
        g_stHpaIntCount.ulSOCStatRecord[3]  = HPA_Read32Reg(0xF711A72C);
        g_stHpaIntCount.ulSOCStatRecord[4]  = HPA_Read32Reg(0xF7410854);
        g_stHpaIntCount.ulSOCStatRecord[5]  = HPA_Read32Reg(0xF741085C);
        g_stHpaIntCount.ulSOCStatRecord[6]  = HPA_Read32Reg(0xF7410864);
        g_stHpaIntCount.ulSOCStatRecord[7]  = HPA_Read32Reg(0xF741086C);
        g_stHpaIntCount.ulSOCStatRecord[8]  = HPA_Read32Reg(0xFDF8A00C);
    }
#endif

    ulBbpStatus = HPA_Read32Reg(g_aulGBBPRegAddr[INT_STA][enModemID]) & OAM_GBBP_FRAME_INT_BIT;
    HPA_Write32RegMask(g_aulGBBPRegAddr[INT_CLR][enModemID], ulBbpStatus );

    ulDrxStatus = HPA_Read32Reg(g_aulGBBPRegAddr[DRX_INT_STA][enModemID]) & (OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT | OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT);

    /* 为了规避BBP在读取TL以及BBP TIMER常开区地址异常bug，在这里进行时钟切换中断读取
       异常规避 */
    if ( 0 == ( OAM_GBBP_FRAME_INT_BIT & ulBbpStatus ) )
    {
        /* 读取时钟切换中断内容异常时处理流程 */
        if( 0 == ulDrxStatus )
        {
            for (i=0; i<3; i++)
            {
                ulDrxStatus = HPA_Read32Reg(g_aulGBBPRegAddr[DRX_INT_STA][enModemID]) & (OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT | OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT);
            }

            if( 0 == ulDrxStatus )
            {
                if (MODEM_ID_0 == enModemID)
                {
                    if (VOS_TRUE == SLEEP_IsAwakeProc(VOS_RATMODE_GSM))
                    {
                        ulDrxStatus |= OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT;
                    }
                    else
                    {
                        ulDrxStatus |= OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT;
                    }
                }
#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
                if (MODEM_ID_1 == enModemID)
                {
                    if (VOS_TRUE == I1_SLEEP_IsAwakeProc(VOS_RATMODE_GSM))
                    {
                        ulDrxStatus |= OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT;
                    }
                    else
                    {
                        ulDrxStatus |= OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT;
                    }
                }
#endif
                /*lint -e661*/
                g_stHpaIntCount.ulBBPSwitchIsrErrorCount[enModemID]++;
                g_stHpaIntCount.ulBBPSwitchIsrErrorSlice[enModemID] = OM_GetSlice();
                /*lint +e661*/
            }
        }
    }
    HPA_Write32RegMask(g_aulGBBPRegAddr[DRX_INT_CLR][enModemID], ulDrxStatus );

    if( 0 != ( OAM_GBBP_FRAME_INT_BIT & ulBbpStatus ) )
    {
        ulBeginSlice = OM_GetSlice();

        while((0 != (HPA_Read32Reg(g_aulGBBPRegAddr[INT_STA][enModemID]) & OAM_GBBP_FRAME_INT_BIT))
            &&(BBP_DRX_INT_SLICE_COUNT > (OM_GetSlice() - ulBeginSlice)))
        {
            ;
        }
    }

    if ((0 != ( OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT & ulDrxStatus))
        || (0 != ( OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT & ulDrxStatus)))
    {
        ulBeginSlice = OM_GetSlice();

        while((0 != (HPA_Read32Reg(g_aulGBBPRegAddr[DRX_INT_STA][enModemID]) & (OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT | OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT)))
            &&(BBP_DRX_INT_SLICE_COUNT > (OM_GetSlice() - ulBeginSlice)))
        {
            ;
        }
    }

    /*lint -e662 -e661*/
    DRV_VICINT_DISABLE(g_aulGBBPIntNO[enModemID]);
    /*lint +e662 +e661*/

    if( 0 != ( OAM_GBBP_FRAME_INT_BIT & ulBbpStatus ) )
    {
        /*lint -e661*/
        g_stHpaIntCount.aulPpIntNum[enModemID]++;
        g_stHpaIntCount.aulPpIntSlice[enModemID] = OM_GetSlice();
        /*lint +e661*/

        if(MODEM_ID_1 == enModemID)
        {
            g_ul2GISRNum1++;

            atomic_inc(&g_stGDsp1MailBoxTransferCount);
        }
        else
        {
            g_ul2GISRNum0++;

            atomic_inc(&g_stGDspMailBoxTransferCount);
        }

        VOS_SmV(g_ulHpaTransferSem);
    }

    if (0 != ( OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT & ulDrxStatus))
    {
        if (MODEM_ID_0 == enModemID)
        {
            SLEEP_32KIsr(VOS_RATMODE_GSM);
        }

/*lint -e718 -e746 修改人:ganlan;检视人:徐铖 51137 */
#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
        if (MODEM_ID_1 == enModemID)
        {
            I1_SLEEP_32KIsr(VOS_RATMODE_GSM);
        }
#endif
/*lint +e718 +e746 修改人:ganlan;检视人:徐铖 51137 */

    }

    if (0 != ( OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT & ulDrxStatus))
    {
        if (MODEM_ID_0 == enModemID)
        {
            SLEEP_NormalClockIsr(VOS_RATMODE_GSM);
        }

/*lint -e718 -e746 修改人:ganlan;检视人:徐铖 51137 */
#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
        if (MODEM_ID_1 == enModemID)
        {
            I1_SLEEP_NormalClockIsr(VOS_RATMODE_GSM);
        }
#endif
/*lint +e718 +e746 修改人:ganlan;检视人:徐铖 51137 */
    }

    /*lint -e662 -e661*/
    DRV_VICINT_ENABLE(g_aulGBBPIntNO[enModemID]);
    /*lint +e662 +e661*/
}

/*****************************************************************************
 Function   : HPA_2GAwakeIsr
 Description: Gsm BBP0 frame and clock isr
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_2GFrameClockIsr0(VOS_VOID)
{
    HPA_2GFrameClockIsr(MODEM_ID_0);
}

/*****************************************************************************
 Function   : HPA_2GAwakeIsr
 Description: Gsm BBP0 frame and clock isr
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_2GFrameClockIsr1(VOS_VOID)
{
    HPA_2GFrameClockIsr(MODEM_ID_1);
}

/*****************************************************************************
 Function   : HPA_2GAwakeIsr
 Description: Gsm Awake isr
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_2GAwakeIsr(MODEM_ID_ENUM_UINT16 enModemID)
{
    VOS_UINT32  ulBeginSlice;

    if(enModemID >= MODEM_ID_BUTT)
    {
        return;
    }

    /*lint -e662 -e661*/
    DRV_VICINT_DISABLE(g_aulGBBPAwakeIntNO[enModemID]);
    /*lint +e662 +e661*/

    if (MODEM_ID_0 == enModemID)
    {
        SLEEP_AwakeIsr(VOS_RATMODE_GSM);
    }

/*lint -e718 -e746 修改人:ganlan;检视人:徐铖 51137 */
#if ( FEATURE_MULTI_MODEM == FEATURE_ON )
    if (MODEM_ID_1 == enModemID)
    {
        I1_SLEEP_AwakeIsr(VOS_RATMODE_GSM);
    }
#endif
/*lint +e718 +e746 修改人:ganlan;检视人:徐铖 51137 */

    HPA_Write32RegMask(g_aulGBBPRegAddr[DRX_INT_CLR][enModemID], OAM_GBBP_WAKE_UP_INT_BIT );

    ulBeginSlice = OM_GetSlice();

    while((0 != (HPA_Read32Reg(g_aulGBBPRegAddr[DRX_INT_STA][enModemID]) & (OAM_GBBP_WAKE_UP_INT_BIT)))
        &&(BBP_DRX_INT_SLICE_COUNT > (OM_GetSlice() - ulBeginSlice)))
    {
        ;
    }

    /*lint -e662 -e661*/
    DRV_VICINT_ENABLE(g_aulGBBPAwakeIntNO[enModemID]);
    /*lint +e662 +e661*/

    return;
}

/*****************************************************************************
 Function   : HPA_2GAwakeIsr0
 Description: ISR of BBP0 Awack ISR
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/

VOS_VOID HPA_2GAwakeIsr0(VOS_VOID)
{
    HPA_2GAwakeIsr(MODEM_ID_0);
}

/*****************************************************************************
 Function   : HPA_2GAwakeIsr0
 Description: ISR of BBP0 Awack ISR
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/

VOS_VOID HPA_2GAwakeIsr1(VOS_VOID)
{
    HPA_2GAwakeIsr(MODEM_ID_1);
}

/*****************************************************************************
 Function   : HPA_3G0MsIsr
 Description: ISR of R99 0ms
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_3G0MsIsr(VOS_VOID)
{
    if (PWRCTRL_COMM_OFF == DRV_PWRCTRL_PWRSTATUSGET((PWC_COMM_MODE_E)VOS_RATMODE_WCDMA, PWC_COMM_MODULE_BBP_DRX, (PWC_COMM_MODEM_E)MODEM_ID_0))
    {
        VOS_ProtectionReboot(DRX_REPORT_BBP_POWERDOWN_ERROR, PS_FILE_ID_HPA_ISR_C, __LINE__,
                VOS_NULL_PTR, 0);

        return;
    }

    /* Clear interrupt */
    HPA_Write32Reg(WBBP_ARM_INT01_CLR_ADDR, 1);

    DRV_VICINT_DISABLE(g_ulWBBP0MsIntNO);

    g_stHpaIntCount.ul0msIntNum++;
    g_stHpaIntCount.ul0msIntSlice = OM_GetSlice();

#ifdef HPA_ITT
    Stub_RttRegGet();

    if ( VOS_NULL_PTR != g_pfnHpaDspIsrStub )
    {
        g_pfnHpaDspIsrStub();
    }
#endif

    /* Read SFN & CFN */
    HPA_ReadCfnSfn();

    /* Trigger HPA_TransferTaskEntry task to transfer msg
       between ARM and DSP.*/
    atomic_inc(&g_stDspMailBoxTransferCount);
    VOS_SmV(g_ulHpaTransferSem);

    g_ul3GISRNum++;

    DRV_VICINT_ENABLE(g_ulWBBP0MsIntNO);

    return ;
}

/*****************************************************************************
 Function   : HPA_3GAwakeIsr
 Description: ISR of W Wake INT
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_3GAwakeIsr(VOS_VOID)
{
    VOS_UINT32      ulRegValue;
    VOS_UINT32      ulBeginSlice;

    DRV_VICINT_DISABLE(g_ulWBBPAwakeIntNO);

    SLEEP_AwakeIsr(VOS_RATMODE_WCDMA);

    ulRegValue = HPA_Read32Reg(WBBP_ARM_INT02_STA_ADDR);

    /* Clear interrupt */
    HPA_Write32Reg(WBBP_ARM_INT02_CLR_1CARD_ADDR, ulRegValue);

    ulBeginSlice = OM_GetSlice();

    while((0 != HPA_Read32Reg(WBBP_ARM_INT02_STA_ADDR))
        &&(BBP_DRX_INT_SLICE_COUNT > (OM_GetSlice() - ulBeginSlice)))
    {
        ;
    }

    DRV_VICINT_ENABLE(g_ulWBBPAwakeIntNO);

    return;
}

/*****************************************************************************
 Function   : HPA_BbpAwakeIsr
 Description: Gsm&Wcdma Drx isr,Only used for asic platform
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_3GSwitchClockIsr(VOS_VOID)
{
    VOS_UINT32  ulBbpStatus;
    VOS_UINT32  ulBeginSlice;

    DRV_VICINT_DISABLE(g_ulWBBPSwtichIntNO);

    ulBbpStatus = HPA_Read32Reg(WBBP_ARM_INT03_STA_ADDR);

    /* Clear interrupt */
    HPA_Write32Reg(WBBP_ARM_INT03_CLR_1CARD_ADDR, ulBbpStatus);

    if(0 != ulBbpStatus)
    {
        ulBeginSlice = OM_GetSlice();

        while((0 != HPA_Read32Reg(WBBP_ARM_INT03_STA_ADDR))
            &&(BBP_DRX_INT_SLICE_COUNT > (OM_GetSlice() - ulBeginSlice)))
        {
            ;
        }

        if( BIT_N(WSLEEP_64M_TO_32K_TYPE_BIT) & ulBbpStatus )
        {
            SLEEP_32KIsr(VOS_RATMODE_WCDMA);
        }
        else if (BIT_N(WSLEEP_32K_TO_64M_TYPE_BIT) & ulBbpStatus)
        {
            SLEEP_NormalClockIsr(VOS_RATMODE_WCDMA);
        }
        else
        {
            ;
        }
    }

    DRV_VICINT_ENABLE(g_ulWBBPSwtichIntNO);

    return;
}

/*lint +e960 修改人：甘兰47350；检视人：xucheng；*/

/*****************************************************************************
 Function   : HPA_Conncet2GISR0
 Description: 2G Isr Connect
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_Conncet2GISR0(VOS_VOID)
{
    BSP_S32 lIntLevel;

    /*CCPU单独复位挂接中断前需清中断*/
    HPA_Write32RegMask(g_aulGBBPRegAddr[INT_CLR][MODEM_ID_0], OAM_GBBP_FRAME_INT_BIT );

    HPA_Write32RegMask(g_aulGBBPRegAddr[DRX_INT_CLR][MODEM_ID_0],
                        OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT|
                        OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT);

    /*挂接BBP0 BSP_INT_TYPE_GBBP中断*/
    lIntLevel = DRV_GET_INT_NO(BSP_INT_TYPE_GBBP);  /*获取BSP_INT_TYPE_GBBP的值*/

    DRV_VICINT_CONNECT((VOID_FUNCPTR *)lIntLevel, (VOIDFUNCPTR)HPA_2GFrameClockIsr0, 0);

    HPA_Clear32RegMask(g_aulGBBPRegAddr[INT_MASK][MODEM_ID_0], OAM_GBBP_FRAME_INT_BIT);    /* 2G Bbp Frame Int */

    HPA_Clear32RegMask(g_aulGBBPRegAddr[DRX_INT_MASK][MODEM_ID_0],
                        OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT|               /* 2G Bbp Switch 32k Int */
                        OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT);              /* 2G Bbp Switch 52M Int */

    DRV_VICINT_ENABLE(lIntLevel);  /*使能BSP_INT_TYPE_GBBP的中断*/

    g_aulGBBPIntNO[MODEM_ID_0] = lIntLevel;

    /*CCPU单独复位挂接中断前需清中断*/
    HPA_Write32RegMask(g_aulGBBPRegAddr[DRX_INT_CLR][MODEM_ID_0], OAM_GBBP_WAKE_UP_INT_BIT );

    /*挂接BBP0 BSP_INT_TYPE_GBBP_AWAKE中断*/
    lIntLevel = DRV_GET_INT_NO(BSP_INT_TYPE_GBBP_AWAKE);  /*获取BSP_INT_TYPE_GBBP_AWAKE的值*/

    DRV_VICINT_CONNECT((VOID_FUNCPTR *)lIntLevel, (VOIDFUNCPTR)HPA_2GAwakeIsr0, 0);

    HPA_Clear32RegMask(g_aulGBBPRegAddr[DRX_INT_MASK][MODEM_ID_0], OAM_GBBP_WAKE_UP_INT_BIT);   /* 2G Bbp Wake up Int */

    DRV_VICINT_ENABLE(lIntLevel);  /*使能BSP_INT_TYPE_GBBP_AWAKE的中断*/

    g_aulGBBPAwakeIntNO[MODEM_ID_0] = lIntLevel;

    return;
}

/*****************************************************************************
 Function   : HPA_Conncet2GISR1
 Description: 2G Isr Connect
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_Conncet2GISR1(VOS_VOID)
{
#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
    BSP_S32 lIntLevel;

    /*CCPU单独复位挂接中断前需清中断*/
    HPA_Write32RegMask(g_aulGBBPRegAddr[INT_CLR][MODEM_ID_1], OAM_GBBP_FRAME_INT_BIT );

    HPA_Write32RegMask(g_aulGBBPRegAddr[DRX_INT_CLR][MODEM_ID_1],
                        OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT|
                        OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT);

    /*挂接BBP1 BSP_INT_TYPE_GBBP中断*/
    lIntLevel = DRV_GET_INT_NO(BSP_INT_TYPE_GBBP1);  /*获取BSP_INT_TYPE_GBBP1的值*/

    DRV_VICINT_CONNECT((VOID_FUNCPTR *)lIntLevel, (VOIDFUNCPTR)HPA_2GFrameClockIsr1, 0);

    HPA_Clear32RegMask(g_aulGBBPRegAddr[INT_MASK][MODEM_ID_1], OAM_GBBP_FRAME_INT_BIT);    /* 2G Bbp Frame Int */

    HPA_Clear32RegMask(g_aulGBBPRegAddr[DRX_INT_MASK][MODEM_ID_1],
                        OAM_GBBP_DRX_CLK_SWITCH_32K_INT_BIT|               /* 2G Bbp Switch 32k Int */
                        OAM_GBBP_DRX_CLK_SWITCH_52M_INT_BIT);              /* 2G Bbp Switch 52M Int */

    DRV_VICINT_ENABLE(lIntLevel);  /*使能BSP_INT_TYPE_GBBP的中断*/

    g_aulGBBPIntNO[MODEM_ID_1] = lIntLevel;

    /*CCPU单独复位挂接中断前需清中断*/
    HPA_Write32RegMask(g_aulGBBPRegAddr[DRX_INT_CLR][MODEM_ID_1], OAM_GBBP_WAKE_UP_INT_BIT );

    /*挂接BBP0 BSP_INT_TYPE_GBBP_AWAKE中断*/
    lIntLevel = DRV_GET_INT_NO(BSP_INT_TYPE_GBBP1_AWAKE);  /*获取BSP_INT_TYPE_GBBP_AWAKE的值*/

    DRV_VICINT_CONNECT((VOID_FUNCPTR *)lIntLevel, (VOIDFUNCPTR)HPA_2GAwakeIsr1, 0);

    HPA_Clear32RegMask(g_aulGBBPRegAddr[DRX_INT_MASK][MODEM_ID_1], OAM_GBBP_WAKE_UP_INT_BIT);   /* 2G Bbp Wake up Int */

    DRV_VICINT_ENABLE(lIntLevel);  /*使能BSP_INT_TYPE_GBBP_AWAKE的中断*/

    g_aulGBBPAwakeIntNO[MODEM_ID_1] = lIntLevel;
#endif

    return;
}

/*****************************************************************************
 Function   : HPA_Conncet3GISR
 Description: 3G Isr Connect
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_Conncet3GISR(VOS_VOID)
{
    BSP_S32 lIntLevel;

    /*CCPU单独复位挂接中断前需清中断*/
    HPA_Write32Reg(WBBP_ARM_INT01_CLR_ADDR, 1);

    /*Connect W BBP interrupt*/
    lIntLevel = DRV_GET_INT_NO(BSP_INT_TYPE_WBBP_0MS);  /*查询BSP_INT_TYPE_WBBP_0MS*/
    DRV_VICINT_CONNECT((VOID_FUNCPTR *)lIntLevel, (VOIDFUNCPTR)HPA_3G0MsIsr, 0);
    HPA_Write32Reg(WBBP_ARM_INT01_MASK_ADDR, 0xFFFFFFFE);
    HPA_Write32Reg(WBBP_ARM_INT01_EN_ADDR, 1);
    DRV_VICINT_ENABLE(lIntLevel);    /*使能BSP_INT_TYPE_WBBP_0MS*/

    g_ulWBBP0MsIntNO = lIntLevel;

    /*CCPU单独复位挂接中断前需清中断*/
    HPA_Write32Reg(WBBP_ARM_INT02_CLR_1CARD_ADDR, 1);

    /*Connect W BBP DRX interrupt*/
    lIntLevel = DRV_GET_INT_NO(BSP_INT_TYPE_WBBP_AWAKE);  /*查询BSP_INT_TYPE_WBBP_AWAKE*/
    DRV_VICINT_CONNECT((VOID_FUNCPTR *)lIntLevel, (VOIDFUNCPTR)HPA_3GAwakeIsr, 0);
    HPA_Write32Reg(WBBP_ARM_INT02_MASK_1CARD_ADDR, 0xFFFFFFFC);
    HPA_Write32Reg(WBBP_ARM_INT02_EN_ADDR, 1);
    DRV_VICINT_ENABLE(lIntLevel);    /*使能BSP_INT_TYPE_WBBP_AWAKE*/

    g_ulWBBPAwakeIntNO = lIntLevel;

    /*CCPU单独复位挂接中断前需清中断*/
    HPA_Write32Reg(WBBP_ARM_INT03_CLR_1CARD_ADDR, 1);

    /*Connect W BBP clock switch interrupt*/
    lIntLevel = DRV_GET_INT_NO(BSP_INT_TYPE_WBBP_SWITCH);  /*查询BSP_INT_TYPE_WBBP_SWITCH*/
    DRV_VICINT_CONNECT((VOID_FUNCPTR *)lIntLevel, (VOIDFUNCPTR)HPA_3GSwitchClockIsr, 0 );
    HPA_Write32Reg(WBBP_ARM_INT03_MASK_1CARD_ADDR, 0xFFFFFFFC);
    HPA_Write32Reg(WBBP_ARM_INT03_EN_ADDR, 1);
    DRV_VICINT_ENABLE(lIntLevel);    /*使能BSP_INT_TYPE_WBBP_SWITCH*/

    g_ulWBBPSwtichIntNO = lIntLevel;

    return;
}

/*****************************************************************************
 Function   : HPA_ShowINTNo
 Description: Show Int NO
 Input      : void
 Return     : void
 Other      :
 *****************************************************************************/
VOS_VOID HPA_ShowINTNo(VOS_VOID)
{
    vos_printf("\r\nZSP  Dog Int NO is   %d", g_ulZSPWatchDogIntNO);
    vos_printf("\r\nHIFI Dog Int NO is   %d", g_ulHIFIWatchDogIntNO);
    vos_printf("\r\nGBBP 0   Int NO is   %d", g_aulGBBPIntNO[MODEM_ID_0]);
    vos_printf("\r\nGBBP 0Aw Int NO is   %d", g_aulGBBPAwakeIntNO[MODEM_ID_0]);
#if  ( FEATURE_MULTI_MODEM == FEATURE_ON )
    vos_printf("\r\nGBBP 1   Int NO is   %d", g_aulGBBPIntNO[MODEM_ID_1]);
    vos_printf("\r\nGBBP 1Aw Int NO is   %d", g_aulGBBPAwakeIntNO[MODEM_ID_1]);
#endif
    vos_printf("\r\nSIMI     Int NO is   %d", g_ulSIMIIntNO);
    vos_printf("\r\nWBBP 0ms Int NO is   %d", g_ulWBBP0MsIntNO);
    vos_printf("\r\nWBBP Aw  Int NO is   %d", g_ulWBBPAwakeIntNO);
    vos_printf("\r\nWBBP Sw  Int NO is   %d", g_ulWBBPSwtichIntNO);

    return;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif /* __cpluscplus */
#endif /* __cpluscplus */


