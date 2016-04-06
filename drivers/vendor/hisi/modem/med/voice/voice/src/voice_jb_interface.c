

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "voice_jb_interface.h"
#include "voice_sjb.h"
#include "HifiOmInterface.h"
#include "om_log.h"
#include "med_drv_timer_hifi.h"
#include "voice_debug.h"
#include "VosPidDef.h"
#include "CodecInterface.h"
#include "ucom_comm.h"
#include "codec_typedefine.h"
#include "voice_proc.h"
#include "voice_mc.h"
#include "ucom_mem_dyn.h"
#include "ucom_nv.h"

#ifdef __cplusplus
#if __cplusplus

extern "C" {
#endif
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
/*lint -e(767)*/
#define THIS_FILE_ID                    OM_FILE_ID_VOICE_JB_INTERFACE_C

/* JB控制参数 */
VOICE_JB_OBJS_STRU                  g_stVoiceJbObjs;

/*****************************************************************************
  3 函数实现
*****************************************************************************/


VOS_VOID VOICE_JB_Init(VOS_VOID)
{
    VOICE_JB_OBJS_STRU              *pstObj            = VOICE_JB_GetJbObjsPtr();

    /* 将JB模块结构体置零 */
    UCOM_MemSet(pstObj, 0, sizeof(VOICE_JB_OBJS_STRU));

    /* 设置默认打包时长为20ms */
    VOICE_JB_SetLtePktTime(VOICE_JB_PKT_TIME_20MS);

}


VOS_UINT32 VOICE_JB_Start(VOS_UINT16  uhwCodecType)
{
    HME_JB_CREATE_STRU               stAjbCreateInfo;
    VOICE_JB_NV_STRU                   stVoiceJbCfg;
    VOS_VOID*                        pBuffForAjb;
    VOS_UINT32                       uwRet;


    UCOM_MemSet(&stVoiceJbCfg, 0, sizeof(VOICE_JB_NV_STRU));

    /* 读取IMS配置NV项 */
    UCOM_NV_Read(en_NV_VoiceJbCfg,
                 &stVoiceJbCfg,
                 sizeof(stVoiceJbCfg));

    /* 设置是否使能AJB */
    VOICE_JB_SetAjbEnable(stVoiceJbCfg.uhwAjbEnable);


    if (VOICE_JB_IsAjbEnable())
    {
        /* 如果AJB的指针不为空，则先将其释放 */
        if (VOICE_JB_GetAjbBuff() != VOS_NULL)
        {
            VOICE_JB_Free();
        }

        /* 申请内存给AJB */
        pBuffForAjb = UCOM_MEM_DYN_Malloc((VOS_UINT32)HME_JB_MEMSIZE, UCOM_MEM_DYN_REQUEST_DDR);
        VOICE_JB_SetAjbBuff(pBuffForAjb);

        /* 初始化AJB */
        UCOM_MemSet(&stAjbCreateInfo, 0, sizeof(HME_JB_CREATE_STRU));
        stAjbCreateInfo.enJBMode = HME_JBMODE_MANUAL;
	    stAjbCreateInfo.stJBParams.uiInitJitterBuf = stVoiceJbCfg.uhwInitBuf;
	    stAjbCreateInfo.stJBParams.uiMinJitterBuf = stVoiceJbCfg.uhwAjbMinBuf;	
	    stAjbCreateInfo.stJBParams.uiMaxJitterBuf = stVoiceJbCfg.uhwAjbMaxBuf;	
        stAjbCreateInfo.pMemAddr = VOICE_JB_GetAjbBuff();
        stAjbCreateInfo.pGetTime = (pSystemTime)VOICE_JB_AjbGetTsInMs;
        stAjbCreateInfo.pLog     = (pWriteLog)VOICE_JB_AjbWriteLog;
        stAjbCreateInfo.pTrace   = (pWriteTrace)VOICE_JB_AjbWriteHookTrace;
        HME_JB_Init(&VOICE_JB_GetAjbModual(), &stAjbCreateInfo);

        /* 注册解码函数 */
        uwRet = (VOS_UINT32)HME_JB_RegDecode(VOICE_JB_GetAjbModual(), VOS_NULL, (pDecode)VOICE_JB_AjbDecode, VOICE_ProcGetSampleRate());

        if (UCOM_RET_SUCC != uwRet)
        {
            return UCOM_RET_FAIL;
        }

        /* 设置AJB打印Log,勾取数据 */
        HME_JB_SetMaintainStatus(VOS_TRUE, VOS_TRUE);
    }
    else
    {
        /* 初始化静态JB */
        VOICE_SJB_InitRx( VOICE_SJB_GetRxSjbCtrl() );
        VOICE_SJB_InitTx( VOICE_SJB_GetTxSjbCtrl() );
    }

    return UCOM_RET_SUCC;
}


VOS_VOID VOICE_JB_Free(VOS_VOID)
{
    UCOM_MEM_DYN_Free(VOICE_JB_GetAjbBuff());
    VOICE_JB_SetAjbBuff(VOS_NULL);
}


VOS_UINT32 VOICE_JB_AddPkt( VOS_VOID* pstPkt )
{
    IMSA_VOICE_RX_DATA_IND_STRU*        pstImsaMsg       = MED_NULL;
    HME_JB_RTP_INFO                     pRtpRtcpPacket;
    VOICE_SJB_RX_MANAGER_STRU*          pstSjbRxCtrl      = VOICE_SJB_GetRxSjbCtrl();
    VOICE_SJB_TX_MANAGER_STRU*          pstSjbTxCtrl      = VOICE_SJB_GetTxSjbCtrl();
    VOS_UINT32                          uwRet            = UCOM_RET_SUCC;
    VOS_UINT32                          uwCpuSr;

    /* 按IMSA_VOICE_RX_DATA_IND消息格式解析 */
    pstImsaMsg = (IMSA_VOICE_RX_DATA_IND_STRU*)pstPkt;

    if (0 == VOICE_JB_IsAjbEnable())
    {
        /* 如果SSRC变化，或者第一次收消息，更改ssrc值 */
        if (pstImsaMsg->ulSSRC != VOICE_JB_GetImsaSsrc())
        {
            /* 第一次设置ssrc */
            if (0 == VOICE_JB_GetImsaSsrc())
            {
                VOICE_JB_SetImsaSsrc(pstImsaMsg->ulSSRC);
            }
            /* ssrc更改，需要重置JB */
            else
            {
                VOICE_JB_SetImsaSsrc(pstImsaMsg->ulSSRC);
                VOICE_SJB_InitRx(pstSjbRxCtrl);
                VOICE_SJB_InitTx(pstSjbTxCtrl);
            }
        }

        /*将消息放入JitterBuffer队列中*/
        uwRet = VOICE_SJB_StorePacketRx(pstSjbRxCtrl, pstPkt);
    }
    else
    {
        /* 将消息转成约定格式送给AJB */
        pRtpRtcpPacket.iCodecType       = pstImsaMsg->usCodecType;
        pRtpRtcpPacket.iFrameLength     = VOICE_JB_PKT_TIME_20MS;
        pRtpRtcpPacket.pPayload         = (VOS_UINT8*) & (pstImsaMsg->usRateMode);
        pRtpRtcpPacket.ulPayloadLength  = VOICE_CODED_FRAME_LENGTH * sizeof(VOS_INT16);
        pRtpRtcpPacket.ucPayloadType    = (VOS_UCHAR)pstImsaMsg->usFrameType;
        pRtpRtcpPacket.usSequenceNumber = pstImsaMsg->usSN;
        pRtpRtcpPacket.ulTimestamp      = pstImsaMsg->ulTS;
        pRtpRtcpPacket.ulSSRC           = pstImsaMsg->ulSSRC;
        pRtpRtcpPacket.uFreq            = (VOS_UINT16)VOICE_ProcGetSampleRate();

        /* 锁中断互斥 */
        uwCpuSr = VOS_SplIMP();
        uwRet = (VOS_UINT32)HME_JB_AddPacket(VOICE_JB_GetAjbModual(), &pRtpRtcpPacket);
        VOS_Splx(uwCpuSr);
    }

    return uwRet;
}
VOS_UINT32 VOICE_JB_GetSjbPkt(VOS_VOID)
{
    VOS_UINT32                   uwRet           = UCOM_RET_SUCC;
    IMSA_VOICE_RX_DATA_IND_STRU  stImsaMsg;
    IMSA_VOICE_RX_DATA_IND_STRU* pstImsaMsg      = &stImsaMsg;

    /*从JB中取一包下行数据*/
    uwRet = VOICE_SJB_GetPacketRx( VOICE_SJB_GetRxSjbCtrl(), pstImsaMsg);

    /* 取数据失败，则送一帧空数据给解码器 */
    if (UCOM_RET_SUCC != uwRet)
    {
        pstImsaMsg->usErrorFlag  = IMSA_VOICE_ERROR;
        pstImsaMsg->usQualityIdx = CODEC_AMR_FQI_QUALITY_BAD;
        OM_LogError1(VOICE_JB_GetSjbPkt_Fail, uwRet);
    }

    /* 将新收到的数据压入解码缓存队列中 */
    VOICE_ProcAddDecSerialWrite(&(stImsaMsg.usRateMode),
                                sizeof(IMS_DL_DATA_OBJ_STRU));

    return UCOM_RET_SUCC;
}
VOS_UINT32 VOICE_JB_GetAjbPcmData(VOS_UINT16 usLength, VOS_VOID *pPcmOut, VOS_UINT32 *puiOutLen)
{
    VOS_UINT32             uwRet;
    VOS_UINT32             uwCpuSr;

    /* 锁中断互斥 */
    uwCpuSr = VOS_SplIMP();
    uwRet = (VOS_UINT32)HME_JB_GetPcmData(VOICE_JB_GetAjbModual(), usLength, pPcmOut, (uint32 *)puiOutLen);
    VOS_Splx(uwCpuSr);

    return uwRet;
}
VOS_VOID VOICE_JB_SendImsaPkt(VOS_VOID *pvEncRslt)
{
    VOICE_SJB_TX_MANAGER_STRU*      pstSjbManager    = VOICE_SJB_GetTxSjbCtrl();
    VOS_UINT16                      uhwCycle;
    VOS_UINT32                      uwRet;
    VOICE_IMSA_TX_DATA_IND_STRU*    pstTxPkt;

    if (VOS_FALSE == VOICE_JB_IsAjbEnable())
    {
        /*将数据存放到JitterBuffer中*/
        VOICE_SJB_StorePacketTx( pstSjbManager, pvEncRslt);

        /*判断是否发送，然后取数据发送*/
        if ( 0 == ( VOICE_SJB_GetPktNumTx(pstSjbManager) % ( pstSjbManager->enPktTime / VOICE_JB_PKT_TIME_20MS ) ) )
        {
            for ( uhwCycle = 0; uhwCycle < ( pstSjbManager->enPktTime / VOICE_JB_PKT_TIME_20MS ); uhwCycle++ )
            {
                uwRet = VOICE_SJB_GetPacketTx( pstSjbManager, pvEncRslt);

                if ( uwRet != UCOM_RET_SUCC)
                {
                    return;
                }

                /*给IMSA发VOICE_IMSA_TX_DATA_IND消息 */
                UCOM_SendOsaMsg(DSP_PID_VOICE,
                                UCOM_PID_DSP_IMSA,
                                pvEncRslt,
                                sizeof(VOICE_IMSA_TX_DATA_IND_STRU));
            }
        }
    }
    else if (VOICE_JB_PKT_TIME_40MS == VOICE_JB_GetLtePktTime())
    {
        if (VOICE_JB_ImsTxPktExist())
        {
            /* 偶数帧，先将缓存帧发出去，再将本帧发出去 */
            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            UCOM_PID_DSP_IMSA,
                            VOICE_JB_GetImsTxPktAddr(),
                            sizeof(VOICE_IMSA_TX_DATA_IND_STRU));

            UCOM_SendOsaMsg(DSP_PID_VOICE,
                            UCOM_PID_DSP_IMSA,
                            pvEncRslt,
                            sizeof(VOICE_IMSA_TX_DATA_IND_STRU));

            UCOM_MEM_DYN_Free(VOICE_JB_GetImsTxPktAddr());
            VOICE_JB_SetImsTxPktExist(VOS_FALSE);
            VOICE_JB_SetImsTxPktAddr(VOS_NULL);
        }
        else
        {
            /* 当前为奇数帧，申请内存，将其先存起来 */
            pstTxPkt = UCOM_MEM_DYN_Malloc(sizeof(VOICE_IMSA_TX_DATA_IND_STRU), UCOM_MEM_DYN_REQUEST_DDR);
            UCOM_MemCpy(pstTxPkt, pvEncRslt, sizeof(VOICE_IMSA_TX_DATA_IND_STRU));
            VOICE_JB_SetImsTxPktExist(VOS_TRUE);
            VOICE_JB_SetImsTxPktAddr((VOS_VOID *)pstTxPkt);
        }
    }
    else
    {
        UCOM_SendOsaMsg(DSP_PID_VOICE,
                        UCOM_PID_DSP_IMSA,
                        pvEncRslt,
                        sizeof(VOICE_IMSA_TX_DATA_IND_STRU));
    }
}
VOS_UINT32 VOICE_JB_SetSjbPktTimeTx(VOS_UINT32 uwPktTime)
{
    VOS_UINT32               uwRet;

    uwRet = VOICE_SJB_SetPktTimeTx( VOICE_SJB_GetTxSjbCtrl(), uwPktTime );
    return uwRet;
}


VOS_INT32 VOICE_JB_AjbWriteLog(VOS_VOID* pLogMessage)
{
    HME_JB_LOG_INFO* pstLogInfo;
    OM_LOG_RPT_LEVEL_ENUM_UINT16 enLevel;

    pstLogInfo = (HME_JB_LOG_INFO*)pLogMessage;

    if (VOICE_JB_HME_LOG_LEVEL_ERR == pstLogInfo->usLevel)
    {
        enLevel = OM_LOG_LEVEL_ERROR;
    }
    else if (VOICE_JB_HME_LOG_LEVEL_WARNING== pstLogInfo->usLevel)
    {
        enLevel = OM_LOG_LEVEL_WARNING;
    }
    else
    {
        enLevel = OM_LOG_LEVEL_INFO;
    }

    OM_LOG_LogReport(enLevel,
                     (VOS_UINT16)pstLogInfo->usFileNo,
                     (VOS_UINT16)pstLogInfo->usLineNo,
                     (VOS_UINT32)pstLogInfo->uiLogId,
                     (VOS_UINT16)pstLogInfo->usFreqency,
                     pstLogInfo->idate1,
                     pstLogInfo->idate2,
                     pstLogInfo->idate3);

    return 0;

}


VOS_INT32 VOICE_JB_AjbGetTsInMs(unsigned int* uwTimeStamp)
{
    VOS_UINT32    uwTS;
    VOS_UINT32    uwHigh;
    VOS_UINT32    uwLow;

    uwTS = DRV_TIMER_ReadSysTimeStamp();

    /* 单位1/32768秒转化成ms,防止溢出 */
    uwHigh = ((uwTS >> 15) & 0x1FFFF) * 1000;
    uwLow  = ((uwTS & 0x7FFF) * 1000) >> 15;

    *uwTimeStamp = uwHigh + uwLow;

    return 0;
}
VOS_INT32 VOICE_JB_AjbWriteHookTrace(VOS_VOID* pTraceData)
{

    HME_JB_TRACE_INFO* pHookData;

    pHookData = (HME_JB_TRACE_INFO*)pTraceData;

    VOICE_DbgSendHook(pHookData->usTraceId,
                      pHookData->pData,
                      pHookData->usDataLen / sizeof(VOS_UINT16));

    return 0;
}


VOS_INT32 VOICE_JB_AjbDecode(void* pstJBmodule, VOS_VOID* pstDecPrm)
{
    VOICE_PROC_CODEC_STRU*      pstCodec       = VOICE_ProcGetCodecPtr();
    HME_JB_DEC_STRU*            pstDecParaAjb  = (HME_JB_DEC_STRU*)pstDecPrm;
    VOS_UINT32                  uwRet          = 0;
    IMS_DL_DATA_OBJ_STRU        stImsPayLoad;

    /* 解码入口LOG,便于区分是AJB的问题还是解码的问题 */
    OM_LogInfo1(VOICE_JB_AjbDecode_Enter, (VOS_UINT32)pstDecPrm);

    /* 如果pcPayload为空指针，需要PLC，输入全0到解码 */
    if (VOS_NULL == pstDecParaAjb->pcPayload)
    {
        UCOM_MemSet(&stImsPayLoad, 0, sizeof(IMS_DL_DATA_OBJ_STRU));
        stImsPayLoad.usErrorFlag  = IMSA_VOICE_ERROR;
        stImsPayLoad.usQualityIdx = CODEC_AMR_FQI_QUALITY_BAD;
        uwRet = VOICE_ProcDecode((VOS_INT16*)&stImsPayLoad, pstDecParaAjb->psOutputData);
    }
    else
    {
        uwRet = VOICE_ProcDecode((VOS_INT16*)pstDecParaAjb->pcPayload, pstDecParaAjb->psOutputData);
    }

    /* 输出帧类型:GOOD/SID/NODATA/OTHER */
    if (CODEC_AMRWB_TYPE_RX_SPEECH_GOOD == pstCodec->stDecInObj.uhwRxTypeForAjb)
    {
        pstDecParaAjb->bPreFrameType = VOICE_FRAME_TYPE_VOICE_GOOD;
    }
    else if ((CODEC_AMRWB_TYPE_RX_SID_FIRST  == pstCodec->stDecInObj.uhwRxTypeForAjb)
             || (CODEC_AMRWB_TYPE_RX_SID_UPDATE == pstCodec->stDecInObj.uhwRxTypeForAjb)
             || (CODEC_AMRWB_TYPE_RX_SID_BAD    == pstCodec->stDecInObj.uhwRxTypeForAjb))
    {
        pstDecParaAjb->bPreFrameType = VOICE_FRAME_TYPE_SID;
    }
    else if (CODEC_AMRWB_TYPE_RX_NO_DATA == pstCodec->stDecInObj.uhwRxTypeForAjb)
    {
        pstDecParaAjb->bPreFrameType = VOICE_FRAME_TYPE_NO_DATA;
    }
    else
    {
        pstDecParaAjb->bPreFrameType = VOICE_FRAME_TYPE_OTHER;
    }

    /* 输出解码后数据长度，采样点为单位 */
    pstDecParaAjb->ulOutputLength = (VOICE_ProcGetSampleRate() * VOICE_JB_PKT_TIME_20MS) / 1000;

    /* 解码出口LOG,便于区分是AJB的问题还是解码的问题 */
    OM_LogInfo(VOICE_JB_AjbDecode_Exit);

    return (VOS_INT32)uwRet;
}

#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

