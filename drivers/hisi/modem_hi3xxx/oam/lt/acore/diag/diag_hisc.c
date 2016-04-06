


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif



#include "diag_port.h"


VOS_VOID diag_HsicCtrlRdCB(VOS_VOID)
{
    diag_PortRdCB(EN_DIAG_HSIC_BEARER_DIAG_CTRL);
}

VOS_VOID diag_HsicAppRdCB(VOS_VOID)
{
    diag_PortRdCB(EN_DIAG_HSIC_BEARER_DIAG_APP);
}


#ifndef FEATURE_USB_ZERO_COPY
VOS_VOID diag_HsicCtrlWrtCB (VOS_CHAR* pDoneBuff, VOS_INT s32DoneSize)
#else
VOS_VOID diag_HsicCtrlWrtCB (VOS_CHAR* pDoneBuff,VOS_CHAR* phyAddr,VOS_INT s32DoneSize)
#endif
{
    diag_PortWrtCB(EN_DIAG_HSIC_BEARER_DIAG_CTRL,pDoneBuff,s32DoneSize);
}

#ifndef FEATURE_USB_ZERO_COPY
VOS_VOID diag_HsicAppWrtCB (VOS_CHAR* pDoneBuff, VOS_INT s32DoneSize)
#else
VOS_VOID diag_HsicAppWrtCB (VOS_CHAR* pDoneBuff,VOS_CHAR* phyAddr,VOS_INT s32DoneSize)
#endif
{
    diag_PortWrtCB(EN_DIAG_HSIC_BEARER_DIAG_APP,pDoneBuff,s32DoneSize);
}

VOS_UINT32 diag_HsicCtrlClose(VOS_VOID)
{
	return diag_PortClose(EN_DIAG_HSIC_BEARER_DIAG_CTRL);
}

VOS_UINT32 diag_HsicAppClose(VOS_VOID)
{
	return diag_PortClose(EN_DIAG_HSIC_BEARER_DIAG_APP);
}

VOS_VOID diag_HsicCtrlEvtCB(ACM_EVT_E  ulEvt)
{
    diag_PortEvtCB(ulEvt,EN_DIAG_HSIC_BEARER_DIAG_CTRL);
}

VOS_VOID diag_HsicAppEvtCB(ACM_EVT_E  ulEvt)
{
    diag_PortEvtCB(ulEvt,EN_DIAG_HSIC_BEARER_DIAG_APP);
}

VOS_UINT32 diag_HsicCtrlOpen(VOS_VOID)
{
	return diag_PortOpen(EN_DIAG_HSIC_BEARER_DIAG_CTRL,\
                         UDI_USB_HSIC_ACM7,\
                         diag_HsicCtrlEvtCB,\
                         diag_HsicCtrlWrtCB,\
                         diag_HsicCtrlRdCB);

}

VOS_UINT32 diag_HsicAppOpen(VOS_VOID)
{
	return diag_PortOpen(EN_DIAG_HSIC_BEARER_DIAG_APP,\
                         UDI_USB_HSIC_ACM8,\
                         diag_HsicAppEvtCB,\
                         diag_HsicAppWrtCB,\
                         diag_HsicAppRdCB);
}


VOS_UINT32 diag_HsicOpen(VOS_VOID)
{
    VOS_UINT32 ulRet = 0;
    VOS_UINT32 i = 0;

    for(i = 0; i < 10; i++)
    {
        ulRet = diag_HsicCtrlOpen();
        if(ERR_MSP_SUCCESS != ulRet)
        {
            vos_printf("[DMS_OpenHsicPort] dmsHsicOm1Open failed.\n");

            VOS_TaskDelay(1000);

            continue;
        }
        else
        {
            break;
        }
    }

    ulRet = diag_HsicAppOpen();
    if(ERR_MSP_SUCCESS != ulRet)
    {
        vos_printf("[DMS_OpenHsicPort] dmsHsicOm2Open failed.\n");

        return ulRet;
    }

    return ulRet;
}

VOS_UINT32 diag_HsicClose(VOS_VOID)
{
	VOS_UINT32 ulRet = 0;

	DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_VCOM_DISABLE,0, 0, 0);

	/*关闭DIAG CTRL通道*/
	ulRet = diag_HsicCtrlClose();
	if(ERR_MSP_SUCCESS!=ulRet)
	{
		DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_VCOM_DISABLE_ERR,0, 0, 1);
	}

	/*关闭DIAG DATA通道*/
	ulRet = diag_HsicAppClose();
	if(ERR_MSP_SUCCESS!=ulRet)
	{
		DIAG_DEBUG_SDM_FUN(EN_DIAG_DEBUG_VCOM_DISABLE_ERR,0, 0, 2);
	}

    return ulRet;
}

VOS_VOID diag_HiscInfo_Init(VOS_VOID)
{
	/*HSIC CTRL*/
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_CTRL].ucChanStat        = ACM_EVT_DEV_SUSPEND;
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_CTRL].ucHdlcFlag        = EN_HDLC_DATA;
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_CTRL].ulCodeDesChanId   = SOCP_CODER_DST_LOM_CNF;
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_CTRL].ulDecodeSrcChanId = SOCP_DECODER_SRC_LOM;
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_CTRL].ulRecivBufLen     = 0;
    g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_CTRL].slPortHandle      = UDI_INVALID_HANDLE;

	/*HSIC IND*/
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_APP].ucChanStat         = ACM_EVT_DEV_SUSPEND;
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_APP].ucHdlcFlag         = EN_HDLC_DATA;
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_APP].ulCodeDesChanId    = SOCP_CODER_DST_LOM_IND;
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_APP].ulDecodeSrcChanId  = 0;
	g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_APP].ulRecivBufLen      = 0;
    g_diagPort[EN_DIAG_HSIC_BEARER_DIAG_APP].slPortHandle       = UDI_INVALID_HANDLE;


}


VOS_UINT32 diag_HiscInit(VOS_VOID)
{
    diag_HiscInfo_Init();

    /* 检查产品是否支持HSIC特性 */
    if (BSP_MODULE_SUPPORT != DRV_GET_HSIC_SUPPORT())
    {
        return ERR_MSP_FAILURE;
    }

    /* 注册HSIC通道打开与关闭回调*/
	if (VOS_TRUE == DRV_GET_HSIC_ENUM_STATUS())
    {
        diag_HsicOpen();
    }
    else
    {
        DRV_HSIC_REGUDI_ENABLECB((HSIC_UDI_ENABLE_CB_T)diag_HsicOpen);
    }

    DRV_HSIC_REGUDI_DISABLECB((HSIC_UDI_DISABLE_CB_T)diag_HsicClose);

    DIAG_PORT_INIT_STATE_SWITCH(EN_DIAG_HSIC_BEARER_DIAG_CTRL,EN_PORT_INIT_SUCC);
    DIAG_PORT_INIT_STATE_SWITCH(EN_DIAG_HSIC_BEARER_DIAG_APP,EN_PORT_INIT_SUCC);

    return ERR_MSP_SUCCESS;

}



#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



