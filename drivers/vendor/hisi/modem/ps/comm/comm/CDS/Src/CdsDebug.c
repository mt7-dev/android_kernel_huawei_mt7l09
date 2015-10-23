


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "CdsDebug.h"
#include "CDS.h"
#include "CdsIpfCtrl.h"
#include "CdsIpFragmentProc.h"


CDS_STATS_INFO_STRU    g_stCdsStats = {0};

VOS_UINT32   g_ulCdsOmSwitch = 0;

/*****************************************************************************
  2 外部函数声明
*****************************************************************************/

/******************************************************************************
   3 私有定义
******************************************************************************/


/******************************************************************************
   4 全局变量定义
******************************************************************************/


/******************************************************************************
   5 函数实现
******************************************************************************/


VOS_VOID CDS_SetDbgRptBearerId(VOS_UINT8  ucEpsbId)
{
    /*LTE默认放在Modem0*/
    g_astCdsEntity[MODEM_ID_0].ucDbgBearerId = ucEpsbId;
    return;
}
VOS_VOID CDS_SetMbmsBearerId(VOS_UINT8  ucEpsbId)
{
    /*LTE默认放在Modem0*/
    g_astCdsEntity[MODEM_ID_0].ucMbmsBearerId = ucEpsbId;
    return;
}
VOS_VOID CDS_SetPktDiscardFlg(MODEM_ID_ENUM_UINT16 enModemId,VOS_UINT32  ulDir, VOS_UINT32 ulFlg)
{
    CDS_ENTITY_STRU         *pstCdsEntity;

    pstCdsEntity = CDS_GetCdsEntity(enModemId);
    if (VOS_NULL_PTR == pstCdsEntity)
    {
        return;
    }

    if (0 == ulDir)
    {
        pstCdsEntity->ulULPktDiscardFlg = ulFlg;
    }
    else
    {
        pstCdsEntity->ulDLPktDiscardFlg = ulFlg;
    }

    return ;
}


VOS_VOID CDS_ShowEventStats(VOS_VOID)
{

    vos_printf("上行收到IPF中断次数                          %d\n",g_stCdsStats.ulIpfULIntNum);
    vos_printf("上行处理IPF中断次数                          %d\n",g_stCdsStats.ulULProcIpfIntNum);
    vos_printf("上行收到ADQ空中断次数                        %d\n",g_stCdsStats.ulUlAdqEmtypIntNum);
    vos_printf("上行处理ADQ空中断次数                        %d\n",g_stCdsStats.ulUlProcAdqEmptyIntNum);
    vos_printf("下行IPF调用内存释放函数回调次数              %d\n",g_stCdsStats.ulIpfDlCallMemFreeNum);
    vos_printf("下行IPF调用内存函数释放内存个数              %d\n",g_stCdsStats.ulIpfDlFreeMemNum);
    vos_printf("下行10ms周期性定时器触发下行处理事件次数     %d\n",g_stCdsStats.ulDL10msTmrTrigEvent);
    vos_printf("下行1ms子帧中断触发下行处理事件次数          %d\n",g_stCdsStats.ulDL1msIntTrigEvent);
    vos_printf("下行UMTS DSP中断触发下行处理事件次数         %d\n",g_stCdsStats.ulDLUmtsIntTrigEvent);
    vos_printf("下行处理下行处理事件次数                     %d\n",g_stCdsStats.ulDLProcEventNum);
    vos_printf("\r\n");

    return;
}


VOS_VOID CDS_ShowUlRdProcStats(VOS_VOID)
{
    vos_printf("上行IPF收到RD个数                            %d\n",g_stCdsStats.ulULIpfRxRDNum);
    vos_printf("上行RD内存转换失败个数                       %d\n",g_stCdsStats.ulULIpfTransMemFailNum);
    vos_printf("上行IPF RD ModemID错误个数                   %d\n",g_stCdsStats.ulULIpfModemIdErrNum);
    vos_printf("上行环回模式释放RD个数                       %d\n",g_stCdsStats.ulULIpfFreePktInLB);
    vos_printf("上行收到IP包个数                             %d\n",g_stCdsStats.ulULIpfRxIpPktNum);
    vos_printf("上行收到IP分片包个数                         %d\n",g_stCdsStats.ulULIpfRxIpSegNum);
    vos_printf("上行收到NDIS包个数                           %d\n",g_stCdsStats.ulULIpfRxNdisPktNum);
    vos_printf("上行IPF过滤错误个数                          %d\n",g_stCdsStats.ulULIpfFilterErrNum);

    vos_printf("\r\n");
    return;
}


VOS_VOID CDS_ShowUlAdqProcStats(VOS_VOID)
{

    vos_printf("上行ADQ分配内存个数                          %d\n",g_stCdsStats.ulULADQAllocMemNum);
    vos_printf("上行ADQ配置失败释放内存个数                  %d\n",g_stCdsStats.ulULADQFreeMemNum);
    vos_printf("上行配置AD 0内存个数                         %d\n",g_stCdsStats.ulULADQCfgAd0Num);
    vos_printf("上行配置AD 1内存个数                         %d\n",g_stCdsStats.ulULADQCfgAd1Num);
    vos_printf("上行配置AD内存总数(AD0+AD1)                  %d\n",g_stCdsStats.ulULADQCfgAdNum);
    vos_printf("上行配置ADQ成功次数                          %d\n",g_stCdsStats.ulULADQCfgAdSucc);
    vos_printf("上行配置ADQ失败次数                          %d\n",g_stCdsStats.ulULADQCfgAdFail);
    vos_printf("上行启动ADQ空保护定时器次数                  %d\n",g_stCdsStats.ulULADQStartEmptyTmrNum);
    vos_printf("上行停止ADQ空保护定时器次数                  %d\n",g_stCdsStats.ulULADQStopEmptyTmrNum);
    vos_printf("上行ADQ空保护定时器超时次数                  %d\n",g_stCdsStats.ulULADQEmptyTmrTimeoutNum);
    vos_printf("上行IPF ADQ空中断个数                        %d\n",g_stCdsStats.ulUlAdqEmtypIntNum);
    vos_printf("上行IPF AD 0引起空中断个数                   %d\n",g_stCdsStats.ulULADQAd0EmptyIntNum);
    vos_printf("上行IPF AD 1引起空中断个数                   %d\n",g_stCdsStats.ulULADQAd1EmptyIntNum);

    vos_printf("\r\n");
    return;
}



VOS_VOID CDS_ShowUlIpPktProcStats(VOS_VOID)
{

    vos_printf("上行接入层收到的IP包总和                     %d\n",g_stCdsStats.ulULASRxIpPktNum);
    vos_printf("上行打桩释放的IP包个数                       %d\n",g_stCdsStats.ulULDirectFreePktNum);
    vos_printf("上行LTE模接收的IP包个数                      %d\n",g_stCdsStats.ulULLTERxPktNum);
    vos_printf("上行GU模接收的IP包个数                       %d\n",g_stCdsStats.ulULGURxPktNum);
    vos_printf("上行NULL模缓存IP包个数                       %d\n",g_stCdsStats.ulULNULLRxPktNum);
    vos_printf("上行接入模式错误释放的IP包个数               %d\n",g_stCdsStats.ulULRanModeErrFreeNum);
    vos_printf("\r\n");

    vos_printf("LTE上行流控释放的IP包个数                    %d\n",g_stCdsStats.ulULLTEProcIpSegFail);
    vos_printf("LTE上行处理IP分片失败IP包个数                %d\n",g_stCdsStats.ulULLTEProcIpSegFail);
    vos_printf("LTE上行发送到LPDCP的IP包个数                 %d\n",g_stCdsStats.ulULLTESendToLPdcpNum);
    vos_printf("LTE上行缓存IP包个数                          %d\n",g_stCdsStats.ulULLTESavePktNum);
    vos_printf("LTE上行ERABM状态错误释放的IP包个数           %d\n",g_stCdsStats.ulULLTEERabmStateErr);
    vos_printf("\r\n");

    vos_printf("GU上行流控释放的IP包个数                     %d\n",g_stCdsStats.ulULGUFcFreePktNum);
    vos_printf("GU上行发送到RABM的IP包个数                   %d\n",g_stCdsStats.ulULGUSendToRabmNum);
    vos_printf("GU上行缓存IP包个数                           %d\n",g_stCdsStats.ulULGUSavePktNum);
    vos_printf("GU上行RABM状态错误释放的IP包个数             %d\n",g_stCdsStats.ulULGURabmStateErr);
    vos_printf("\r\n");

    vos_printf("上行缓存入队成功个数                         %d\n",g_stCdsStats.ulULBuffEnQueSucc);
    vos_printf("上行缓存入队失败个数                         %d\n",g_stCdsStats.ulULBuffEnQueFail);
    vos_printf("上行缓存软过滤失败个数                       %d\n",g_stCdsStats.ulULBuffSoftFilterFail);
    vos_printf("上行缓存发送IP包成功个数                     %d\n",g_stCdsStats.ulULBuffSendPktSuccNum);
    vos_printf("上行缓存释放IP包个数                         %d\n",g_stCdsStats.ulULBuffFreePktNum);
    vos_printf("\r\n");

    vos_printf("上行IP包分类统计总和\n");
    vos_printf("上行收到NDCLIENT包个数                       %d\n",g_stCdsStats.ulULRecvNdPktNum);
    vos_printf("上行收到环回数据包个数                       %d\n",g_stCdsStats.ulLBUlSendPktNum);
    vos_printf("上行收到RD数据包总数                         %d\n",g_stCdsStats.ulULIpfRxIpPktNum + g_stCdsStats.ulULIpfRxIpSegNum);
    vos_printf("上行缓存数据重新发送个数                     %d\n",g_stCdsStats.ulULBuffSendPktSuccNum);
    vos_printf("上行按模式分发的数据包总和(ND+LB+IP+BUFF)    %d\n",g_stCdsStats.ulULASRxIpPktNum);

    return;
}


VOS_VOID CDS_ShowDlSduProcStats(VOS_VOID)
{

    vos_printf("下行SDU数传统计\n\n");

    vos_printf("下行GSM模接收SDU个数                         %d\n",g_stCdsStats.ulDLGSMRxSduNum);
    vos_printf("下行GSM模接收IMS数据包个数                   %d\n",g_stCdsStats.ulDLGSMRxImsNum);
    vos_printf("下行GSM模流控丢弃SDU个数                     %d\n",g_stCdsStats.ulDLGSMFcFreeSduNum);
    vos_printf("下行GSM模环回模式下接收SDU个数               %d\n",g_stCdsStats.ulLBDLRxSduInGSM);
    vos_printf("下行GSM获取缺省RABID失败个数                 %d\n",g_stCdsStats.ulDLGSMGetDeftRabIdFail);
    vos_printf("下行GSM模入队成功个数                        %d\n",g_stCdsStats.ulDLGSMEnQueSucc);
    vos_printf("下行GSM模入队失败个数                        %d\n",g_stCdsStats.ulDLGSMEnQueFail);
    vos_printf("\r\n");

    vos_printf("下行UMTS模接收SDU个数                        %d\n",g_stCdsStats.ulDLUMTSRxSduNum);
    vos_printf("下行UMTS模接收IMS数据包个数                  %d\n",g_stCdsStats.ulDLUMTSRxImsNum);
    vos_printf("下行UMTS模流控丢弃SDU个数                    %d\n",g_stCdsStats.ulDLUMTSFcFreeSduNum);
    vos_printf("下行UMTS模环回模式下接收SDU个数              %d\n",g_stCdsStats.ulLBDLRxSduInUMTS);
    vos_printf("下行UMTS模打桩直接释放SDU个数                %d\n",g_stCdsStats.ulDLUMTSDirectFreeSduNum);
    vos_printf("下行UMTS获取缺省RABID失败个数                %d\n",g_stCdsStats.ulDLUMTSGetDeftRabIdFail);
    vos_printf("下行UMTS模入队成功个数                       %d\n",g_stCdsStats.ulDLUMTSEnQueSucc);
    vos_printf("下行UMTS模入队失败个数                       %d\n",g_stCdsStats.ulDLUMTSEnQueFail);
    vos_printf("\r\n");

    vos_printf("下行LTE模接收SDU个数                         %d\n",g_stCdsStats.ulDLLteRxSduNum);
    vos_printf("下行LTE模接收IMS数据包个数                   %d\n",g_stCdsStats.ulDLLteRxImsNum);
    vos_printf("下行LTE模流控丢弃SDU个数                     %d\n",g_stCdsStats.ulDLLteFcFreeSduNum);
    vos_printf("下行LTE模打桩直接释放SDU个数                 %d\n",g_stCdsStats.ulDLLteDirectFreeSduNum);
    vos_printf("下行LTE模环回模式下接收SDU个数               %d\n",g_stCdsStats.ulLBDLRxSduInLte);
    vos_printf("下行LTE获取对应EPSBID失败个数                %d\n",g_stCdsStats.ulDLLteGetEpsbIDFail);
    vos_printf("下行LTE获取缺省EPSBID失败个数                %d\n",g_stCdsStats.ulDLLteGetDeftEpsbIDFail);
    vos_printf("下行LTE接收L2 Debug数据包个数                %d\n",g_stCdsStats.ulDLLteRxDbgPkt);
    vos_printf("下行LTE接收MBMS数据包个数                    %d\n",g_stCdsStats.ulDLLteRxMbmsSdu);
    vos_printf("下行LTE模入队成功个数                        %d\n",g_stCdsStats.ulDLLteEnQueSucc);
    vos_printf("下行LTE模入队失败个数                        %d\n",g_stCdsStats.ulDLLteEnQueFail);


    vos_printf("\r\n");

    vos_printf("下行SDU队列当前缓存数据包个数               %d\n",LUP_QueCnt(CDS_GET_IPF_DL_SDU_QUE()));
    vos_printf("下行SDU入队成功总数                          %d\n",g_stCdsStats.ulDLEnSduQueSucc);
    vos_printf("下行SDU入队失败总数                          %d\n",g_stCdsStats.ulDLEnSduQueFail);
    vos_printf("\r\n");

    return;
}


VOS_VOID CDS_ShowDlIpfProcStats(VOS_VOID)
{
    vos_printf("下行IPF处理统计\n\n");

    vos_printf("下行配置IPF没有BD次数                       %d\n",g_stCdsStats.ulDLIpfCfgHaveNoBD);
    vos_printf("下行配置IPF没有CD次数                       %d\n",g_stCdsStats.ulDLIpfCfgHaveNoCD);
    vos_printf("下行配置IPF CD不够次数                      %d\n",g_stCdsStats.ulDLIpfCfgCDNotEnough);
    vos_printf("下行配置BD成功个数                          %d\n",g_stCdsStats.ulDLIpfCfgBDSucc);
    vos_printf("下行配置BD失败个数                          %d\n",g_stCdsStats.ulDLIpfCfgBDFail);
    vos_printf("\r\n");

    vos_printf("下行桥接模式保存源内存成功个数              %d\n",g_stCdsStats.ulDLIpfSaveSrcMemSucc);
    vos_printf("下行桥接模式保存源内存失败个数              %d\n",g_stCdsStats.ulDLIpfSaveSrcMemFail);
    vos_printf("下行桥接模式源内存释放队列当前个数          %d\n",LUP_QueCnt(CDS_IPF_DL_SRCMEM_FREE_QUE()));
    vos_printf("下行桥接模式释放源内存成功个数              %d\n",g_stCdsStats.ulDLIpfFreeSrcMemNum);
    vos_printf("下行桥接模式释放源内存失败个数              %d\n",g_stCdsStats.ulDLIpfFreeSrcMemErr);
    vos_printf("\r\n");

    vos_printf("下行非桥接模式IPF分配内存失败次数           %d\n",g_stCdsStats.ulDLIpfCfgBDAllocMemFail);
    vos_printf("下行非桥接模式SDU长度错误个数               %d\n",g_stCdsStats.ulDLIpfCfgBDSduLenErr);
    vos_printf("下行非桥接模式IPF调用内存释放函数回调次数   %d\n",g_stCdsStats.ulIpfDlCallMemFreeNum);
    vos_printf("下行非桥接模式释放内存个数                  %d\n",g_stCdsStats.ulIpfDlFreeMemNum);

    vos_printf("\r\n");

    return;
}



VOS_VOID CDS_ShowLBProcStats(VOS_VOID)
{
    vos_printf("环回模式处理统计\n\n");
    vos_printf("环回下行从GSM接收的SDU个数                  %d\n",g_stCdsStats.ulLBDLRxSduInGSM);
    vos_printf("环回下行从UMTS接收的SDU个数                 %d\n",g_stCdsStats.ulLBDLRxSduInUMTS);
    vos_printf("环回下行从LTE接收的SDU个数                  %d\n",g_stCdsStats.ulLBDLRxSduInLte);
    vos_printf("环回下行申请内存失败的个数                  %d\n",g_stCdsStats.ulLBDLAllocMemFail);
    vos_printf("环回下行入队成功个数                        %d\n",g_stCdsStats.ulLBDDLEnQueSucc);
    vos_printf("环回下行入队失败个数                        %d\n",g_stCdsStats.ulLBDLEnQueFail);
    vos_printf("环回上行软过滤失败个数                      %d\n",g_stCdsStats.ulLBULSoftFilterFail);
    vos_printf("环回上行发送到数据包个数                    %d\n",g_stCdsStats.ulLBUlSendPktNum);
    vos_printf("环回上行清空队列释放数据包个数              %d\n",g_stCdsStats.ulLBUlClearPktNum);

    vos_printf("\r\n");

    return;
}



VOS_VOID CDS_ShowModemGUProcStats(VOS_UINT16 usModemId)
{

    if (usModemId >= MODEM_ID_BUTT)
    {
        vos_printf("ModemId非法。输入ModemId=%d\n",usModemId);
        return;
    }

    vos_printf("Modem %d上的GU数传统计信息\n",usModemId);
    vos_printf("上行GU模接收的IP包个数                       %d\n",g_stCdsStats.astModemStats[usModemId].ulULGURxPktNum);
    vos_printf("上行GU流控释放的IP包个数                     %d\n",g_stCdsStats.astModemStats[usModemId].ulULGUFcFreePktNum);
    vos_printf("上行GU发送到RABM的IP包个数                   %d\n",g_stCdsStats.astModemStats[usModemId].ulULGUSendToRabmNum);
    vos_printf("上行GU缓存IP包个数                           %d\n",g_stCdsStats.astModemStats[usModemId].ulULGUSavePktNum);
    vos_printf("上行GU RABM状态错误释放的IP包个数            %d\n",g_stCdsStats.astModemStats[usModemId].ulULGURabmStateErr);
    vos_printf("\r\n");

    vos_printf("下行GSM模接收SDU个数                         %d\n",g_stCdsStats.astModemStats[usModemId].ulDLGSMRxSduNum);
    vos_printf("下行GSM模接收IMS数据包个数                   %d\n",g_stCdsStats.astModemStats[usModemId].ulDLGSMRxImsNum);
    vos_printf("下行GSM模流控丢弃SDU个数                     %d\n",g_stCdsStats.astModemStats[usModemId].ulDLGSMFcFreeSduNum);
    vos_printf("下行GSM模环回模式下接收SDU个数               %d\n",g_stCdsStats.astModemStats[usModemId].ulLBDLRxSduInGSM);
    vos_printf("下行GSM模入队成功个数                        %d\n",g_stCdsStats.astModemStats[usModemId].ulDLGSMEnQueSucc);
    vos_printf("下行GSM模入队失败个数                        %d\n",g_stCdsStats.astModemStats[usModemId].ulDLGSMEnQueFail);
    vos_printf("\r\n");

    vos_printf("下行UMTS模接收SDU个数                        %d\n",g_stCdsStats.astModemStats[usModemId].ulDLUMTSRxSduNum);
    vos_printf("下行UMTS模接收IMS数据包个数                  %d\n",g_stCdsStats.astModemStats[usModemId].ulDLUMTSRxImsNum);
    vos_printf("下行UMTS模流控丢弃SDU个数                    %d\n",g_stCdsStats.astModemStats[usModemId].ulDLUMTSFcFreeSduNum);
    vos_printf("下行UMTS模环回模式下接收SDU个数              %d\n",g_stCdsStats.astModemStats[usModemId].ulLBDLRxSduInUMTS);
    vos_printf("下行UMTS模打桩直接释放SDU个数                %d\n",g_stCdsStats.astModemStats[usModemId].ulDLUMTSDirectFreeSduNum);
    vos_printf("下行UMTS模入队成功个数                       %d\n",g_stCdsStats.astModemStats[usModemId].ulDLUMTSEnQueSucc);
    vos_printf("下行UMTS模入队失败个数                       %d\n",g_stCdsStats.astModemStats[usModemId].ulDLUMTSEnQueFail);
    vos_printf("\r\n");

    vos_printf("\r\n");

    return;
}



VOS_VOID CDS_ShowCdsEntityInfo(VOS_UINT16 usModemId)
{
    char    strRanMode[][20]  = {"NULL","LTE","GU","无效接入模式"};
    char    strLBState[][20]  = {"正常模式","环回模式"};


    CDS_ENTITY_STRU         *pstCdsEntity;
    VOS_UINT32               ulCnt;

    pstCdsEntity = CDS_GetCdsEntity(usModemId);
    if (VOS_NULL_PTR == pstCdsEntity)
    {
        vos_printf("ModemId非法。输入ModemId=%d\n",usModemId);
        return;
    }

    vos_printf("显示Modem%d的CDS实体状态  \r\n",usModemId);
    vos_printf("当前接入模式              : %s\r\n",strRanMode[pstCdsEntity->enRanMode]);
    vos_printf("上行SR触发标志            : %d\r\n",pstCdsEntity->ulServiceReqFlg);
    vos_printf("上行缓存队列缓存个数      : %d\r\n",LUP_QueCnt(pstCdsEntity->pstUlDataQue));
    vos_printf("上行缓存数据保护定时器    : 0x%x\r\n",pstCdsEntity->astTimer[CDS_TMR_ID_UL_DATA_PROCTECT].pstTmrHdr);
    vos_printf("下行10ms周期性定时器      : 0x%x\r\n",pstCdsEntity->astTimer[CDS_TMR_ID_DL_10MS_PERIODIC_TMR].pstTmrHdr);
    vos_printf("环回模式状态              : %s\r\n",strLBState[pstCdsEntity->ulTestModeFlg]);
    vos_printf("环回模式类型              : %d\r\n",pstCdsEntity->ulLoopBackMode);
    vos_printf("环回模式B定时器时长       : %d\r\n",pstCdsEntity->astTimer[CDS_TMR_ID_LB_MODE_B_TMR].ulTmrLen);
    vos_printf("环回队列当前缓存SDU       : %d\r\n",LUP_QueCnt(pstCdsEntity->pstLBModeBQue));

    vos_printf("上行数据包直接丢弃开关    : %d\r\n",pstCdsEntity->ulULPktDiscardFlg);
    vos_printf("下行数据包直接丢弃开关    : %d\r\n",pstCdsEntity->ulDLPktDiscardFlg);

    vos_printf("\r\n");

    vos_printf("显示Modem%d的IMS承载信息  \r\n",usModemId);

    vos_printf("IMS端口号范围             :%d - %d\r\n",pstCdsEntity->stImsPortInfo.usMinImsPort,
                                                pstCdsEntity->stImsPortInfo.usMaxImsPort);

    vos_printf("IMS承载总数               : %d\r\n",pstCdsEntity->ulImsBearerNum);
    for (ulCnt = 0; ulCnt < pstCdsEntity->ulImsBearerNum; ulCnt ++)
    {
        vos_printf("IMS承载ID                 : %d\r\n",pstCdsEntity->astImsBearerInfo[ulCnt].ucEpsbId);
        vos_printf("IMS承载类型               : %d\r\n",pstCdsEntity->astImsBearerInfo[ulCnt].enBearerType);
    }

    vos_printf("\r\n");
    return;
}


VOS_VOID CDS_PrintIpv4Addr(const IPV4_ADDR_UN  *pstIpv4Addr)
{
    CDS_ASSERT(VOS_NULL_PTR != pstIpv4Addr);

    vos_printf("%d.%d.%d.%d\r\n", pstIpv4Addr->aucIPAddr[0],
                           pstIpv4Addr->aucIPAddr[1],
                           pstIpv4Addr->aucIPAddr[2],
                           pstIpv4Addr->aucIPAddr[3]);
    return;
}


VOS_VOID CDS_PrintIpv6Addr(const IPV6_ADDR_UN  *pstIpv6Addr)
{

    VOS_UINT32       ulCnt;

    CDS_ASSERT(VOS_NULL_PTR != pstIpv6Addr);

    for (ulCnt = 0; ulCnt < IPV6_ADDR_LEN - 1; ulCnt ++)
    {
        vos_printf("%.2x:",pstIpv6Addr->aucIPAddr[ulCnt]);
    }

    vos_printf("%.2x\r\n",pstIpv6Addr->aucIPAddr[ulCnt]);

    return;
}

VOS_VOID CDS_ShowSoftFilterIPv4Info(const CDS_SOFTFILTER_INFO_STRU *pstSoftFilter)
{
    /*Service Type*/
    if (PS_TRUE == pstSoftFilter->bitOpTypeOfService)
    {
        vos_printf("TFT Service Type          : %d\r\n",pstSoftFilter->ucTypeOfService);
        vos_printf("TFT Service Type Mask     : %d\r\n",pstSoftFilter->ucTypeOfServiceMask);
    }

    /*协议ID*/
    if (PS_TRUE == pstSoftFilter->bitOpProtocolId)
    {
        vos_printf("TFT Protocol ID           : %d\r\n",pstSoftFilter->ucTypeOfService);
    }

    /*源IP地址信息*/
    if (PS_TRUE == pstSoftFilter->bitOpLcIpv4AddrAndMask)
    {
        vos_printf("TFT IPv4 Src Address      : ");
        CDS_PrintIpv4Addr(&(pstSoftFilter->unLocalIpv4Addr));
        vos_printf("TFT IPv4 Src Address Mask : ");
        CDS_PrintIpv4Addr(&(pstSoftFilter->unLocalIpv4Mask));
    }

    /*目的IP地址信息*/
    if (PS_TRUE == pstSoftFilter->bitOpRmtIpv4AddrAndMask)
    {
        vos_printf("TFT IPv4 Dest Address     : ");
        CDS_PrintIpv4Addr(&(pstSoftFilter->unRmtIpv4Addr));
        vos_printf("TFT IPv4 Dest Address Mask: ");
        CDS_PrintIpv4Addr(&(pstSoftFilter->unRmtIpv4Mask));
    }

    return;
}



VOS_VOID CDS_ShowSoftFilterIPv6Info(const CDS_SOFTFILTER_INFO_STRU *pstSoftFilter)
{

    /*Service Type*/
    if (PS_TRUE == pstSoftFilter->bitOpTypeOfService)
    {
        vos_printf("TFT Service Type          : %d\r\n",pstSoftFilter->ucTypeOfService);
        vos_printf("TFT Service Type Mask     : %d\r\n",pstSoftFilter->ucTypeOfServiceMask);
    }

    /*协议ID*/
    if (PS_TRUE == pstSoftFilter->bitOpProtocolId)
    {
        vos_printf("TFT Protocol ID           : %d\r\n",pstSoftFilter->ucTypeOfService);
    }

    /*FlowLable*/
    if (PS_TRUE == pstSoftFilter->bitOpFlowLabelType)
    {
        vos_printf("TFT FlowLable Type        : 0x%.8x\r\n",pstSoftFilter->ulFlowLabelType);
    }

    /*源IPv6地址*/
    if (PS_TRUE == pstSoftFilter->bitOpLcIpv6AddrAndMask)
    {
        vos_printf("TFT IPv6 Source Address    \r\n");
        CDS_PrintIpv6Addr(&(pstSoftFilter->unLocalIpv6Addr));
        vos_printf("TFT IPv6 Source Address Mask \r\n");
        CDS_PrintIpv6Addr(&(pstSoftFilter->unLocalIpv6Mask));
    }

    /*目的IPv6地址*/
    if (PS_TRUE == pstSoftFilter->bitOpRmtIpv6AddrAndMask)
    {
        vos_printf("TFT IPv6 Dest Address    \r\n");
        CDS_PrintIpv6Addr(&(pstSoftFilter->unRmtIpv6Addr));
        vos_printf("TFT IPv6 Dest Address Mask \r\n");
        CDS_PrintIpv6Addr(&(pstSoftFilter->unRmtIpv6Mask));
    }

    return;
}
VOS_VOID CDS_ShowSoftFilterTransportLayerInfo(const CDS_SOFTFILTER_INFO_STRU *pstSoftFilter)
{

    /*SPI*/
    if (PS_TRUE == pstSoftFilter->bitOpSecuParaIndex)
    {
        vos_printf("TFT IPSec SPI             : 0x%.8x\r\n",pstSoftFilter->ulSecuParaIndex);
    }

    /*源端口信息*/
    if (PS_TRUE == pstSoftFilter->bitOpSingleLocalPort)
    {
        vos_printf("TFT Source Port           : %d\r\n",pstSoftFilter->usSingleLcPort);
    }

    if (PS_TRUE == pstSoftFilter->bitOpLocalPortRange)
    {
        vos_printf("TFT Source Port Range     : [%d,%d]\r\n",pstSoftFilter->usLcPortLowLimit,pstSoftFilter->usLcPortHighLimit);
    }

    /*目的端口信息*/
    if (PS_TRUE == pstSoftFilter->bitOpSingleRemotePort)
    {
        vos_printf("TFT Dest Port             : %d\r\n",pstSoftFilter->usSingleRmtPort);
    }

    if (PS_TRUE == pstSoftFilter->bitOpRemotePortRange)
    {
        vos_printf("TFT Dest Port Range       : [%d,%d]\r\n",pstSoftFilter->usRmtPortLowLimit,pstSoftFilter->usRmtPortHighLimit);
    }

    return;
}
VOS_VOID CDS_ShowUlSoftFilter(VOS_UINT16 usModemId)
{

    CDS_ENTITY_STRU             *pstCdsEntity;
    CDS_SOFTFILTER_INFO_STRU    *pstSoftFilter;
    VOS_UINT32                   ulCnt;

    pstCdsEntity = CDS_GetCdsEntity(usModemId);
    if (VOS_NULL_PTR == pstCdsEntity)
    {
        vos_printf("ModemId非法。输入ModemId=%d\n",usModemId);
        return;
    }

    vos_printf("显示Modem%d的上行软过滤器，总数为%d  \r\n",usModemId,pstCdsEntity->ulUlSoftFilterNum);

    for (ulCnt = 0; ulCnt < pstCdsEntity->ulUlSoftFilterNum; ulCnt ++)
    {
        pstSoftFilter = &(pstCdsEntity->astUlSoftFilter[ulCnt]);
        vos_printf("上行软过滤器%d详细信息\r\n",ulCnt);
        vos_printf("上行软过滤器ID            : %d\r\n",pstSoftFilter->ulPacketFilterId);
        vos_printf("上行软过滤器方向          : %d\r\n",pstSoftFilter->enDirection);
        vos_printf("上行软过滤器类型          : %d\r\n",pstSoftFilter->enFilterType);
        vos_printf("上行软过滤器优先级        : %d\r\n",pstSoftFilter->usPrecedence);
        vos_printf("上行软过滤器对应的承载ID  : %d\r\n",pstSoftFilter->ucRabId);

        if (CDS_SOFTFILTER_TYPE_IPV4 == pstSoftFilter->enFilterType)
        {
            CDS_ShowSoftFilterIPv4Info(pstSoftFilter);
        }
        else
        {
            CDS_ShowSoftFilterIPv6Info(pstSoftFilter);
        }

        CDS_ShowSoftFilterTransportLayerInfo(pstSoftFilter);

        vos_printf("\r\n");

    }


    vos_printf("\r\n");
    return;
}



VOS_VOID CDS_ShowUlSoftFilterStats(VOS_VOID)
{

    vos_printf("上行软过滤数据包个数                         %d\n",g_stCdsStats.ulULSoftFilterPktNum);
    vos_printf("上行软过滤IPV4数据包个数                     %d\n",g_stCdsStats.ulULSoftFilterIPv4Num);
    vos_printf("上行软过滤IPV6数据包个数                     %d\n",g_stCdsStats.ulULSoftFilterIPv6Num);
    vos_printf("上行软过滤失败个数                           %d\n",g_stCdsStats.ulULSoftFilterFailNum);
    vos_printf("上行软过滤IP分片个数                         %d\n",g_stCdsStats.ulULSoftFilterIPSegNum);
    vos_printf("上行软过滤IP分片处理成功个数                 %d\n",g_stCdsStats.ulULSoftFilterIPSegSucc);
    vos_printf("上行软过滤IP分片处理失败个数                 %d\n",g_stCdsStats.ulULSoftFilterIPSegFail);

    vos_printf("\r\n");
    return;
}



VOS_VOID CDS_ShowIMSStats(VOS_VOID)
{
    vos_printf("IMS上行收包函数调用次数                      %d\n",g_stCdsStats.ulImsULRxFunCallNum);
    vos_printf("IMS上行收包函数入参非法次数                  %d\n",g_stCdsStats.ulImsULRxFunParaErr);

    vos_printf("IMS上行接收IMS NIC数据包个数                 %d\n",g_stCdsStats.ulImsULRxNicPktNum);
    vos_printf("IMS上行申请内存失败个数                      %d\n",g_stCdsStats.ulImsULAllocMemFail);
    vos_printf("IMS上行入队失败个数                          %d\n",g_stCdsStats.ulImsULEnQueFail);
    vos_printf("IMS上行入队成功个数                          %d\n",g_stCdsStats.ulImsULEnQueSucc);
    vos_printf("IMS上行触发IMS_PROC事件次数                  %d\n",g_stCdsStats.ulImsULTrigImsEvent);
    vos_printf("IMS上行处理IMS_PROC事件次数                  %d\n",g_stCdsStats.ulImsULProcImsEvent);
    vos_printf("IMS上行软过滤失败次数                        %d\n",g_stCdsStats.ulImsULSoftFilterFail);
    vos_printf("IMS上行数据包发送到空口个数                  %d\n",g_stCdsStats.ulImsULSendToRan);

    vos_printf("IMS下行接收IMS数据包个数                     %d\n",g_stCdsStats.ulImsDLRxImsPkt);
    vos_printf("IMS下行接收到NDCLIENT数据包个数              %d\n",g_stCdsStats.ulImsDLNdPktNum);
    vos_printf("IMS下行申请内存失败个数                      %d\n",g_stCdsStats.ulImsDLAllocMemFail);
    vos_printf("IMS下行获取目的端口号失败个数                %d\n",g_stCdsStats.ulImsDLGetDestPortFail);
    vos_printf("IMS下行发送到IMS NIC数据包失败个数           %d\n",g_stCdsStats.ulImsDLSendToNicFail);
    vos_printf("IMS下行发送到IMS NIC数据包成功个数           %d\n",g_stCdsStats.ulImsDLSendToNicSucc);

    vos_printf("IMS下行接收Ut数据包个数                     %d\n",g_stCdsStats.ulImsDLRxUtPkt);
    vos_printf("IMS下行Ut数据包处理成功个数                 %d\n",g_stCdsStats.ulImsDLUtPktProcSuccNum);
    vos_printf("IMS下行Ut数据包处理失败个数                 %d\n",g_stCdsStats.ulImsDLUtPktProcErrNum);

    vos_printf("\r\n");
    return;
}


VOS_VOID CDS_ShowIpV4FragmentBuffInfo(VOS_VOID)
{

    VOS_UINT32                         ulCnt;
    CDS_IPV4_FRAGMENT_BUFF_STRU       *pstIpv4FragBuff;
    CDS_IPV4_FRAGMENT_INFO_STRU       *pstIpv4FragInfo;

    pstIpv4FragBuff = CDS_GET_IPV4_FRAGMENT_BUFF();
    for (ulCnt = 0; ulCnt < CDS_IPV4_FRAGMENT_BUFF_SIZE; ulCnt ++)
    {
        pstIpv4FragInfo = &(pstIpv4FragBuff->astSegBuff[ulCnt]);
        vos_printf("\n IPV4 Fragment Buffer Index : %d\n",ulCnt);
        vos_printf("EPS Bearer  ID:                 %d\n", pstIpv4FragInfo->ucEpsbID);
        vos_printf("Source IP Addr:                 ");
        CDS_PrintIpv4Addr(&(pstIpv4FragInfo->unSrcAddr));
        vos_printf("Dest   IP Addr:                 ");
        CDS_PrintIpv4Addr(&(pstIpv4FragInfo->unDstAddr));
        vos_printf("IP Fragment ID:                 %d\n", pstIpv4FragInfo->usIpIdentity);
    }

    return;
}


VOS_VOID CDS_ShowIpV6FragmentBuffInfo(VOS_VOID)
{

    VOS_UINT32                         ulCnt;
    CDS_IPV6_FRAGMENT_BUFF_STRU       *pstIpv6FragBuff;
    CDS_IPV6_FRAGMENT_INFO_STRU       *pstIpv6FragInfo;

    pstIpv6FragBuff = CDS_GET_IPV6_FRAGMENT_BUFF();
    for (ulCnt = 0; ulCnt < CDS_IPV6_FRAGMENT_BUFF_SIZE; ulCnt ++)
    {
        pstIpv6FragInfo = &(pstIpv6FragBuff->astSegBuff[ulCnt]);
        vos_printf("\n IPV6 Fragment Buffer Index : %d\n",ulCnt);
        vos_printf("EPS Bearer  ID:    %d\n", pstIpv6FragInfo->ucEpsbID);
        vos_printf("IP Fragment ID:    %d\n", pstIpv6FragInfo->ulIdentity);
        vos_printf("Src IPV6 Addr :    ");
        CDS_PrintIpv6Addr(&(pstIpv6FragInfo->unSrcAddr));
        vos_printf("Dst IPV6 Addr :    ");
        CDS_PrintIpv6Addr(&(pstIpv6FragInfo->unDstAddr));
    }

    return;
}



VOS_VOID CDS_ShowIpFragStats(VOS_VOID)
{

    vos_printf("接收IPV4 分片的个数               %d\n",g_stCdsIpFragBuff.stStats.ulRxIpv4Frag);
    vos_printf("接收IPV4 IPF过滤成功的个数        %d\n",g_stCdsIpFragBuff.stStats.ulRxIpv4IpfSucc);
    vos_printf("接收IPV4 IPF过滤失败的个数        %d\n",g_stCdsIpFragBuff.stStats.ulRxIpv4IpfFail);
    vos_printf("接收IPV6 分片的个数               %d\n",g_stCdsIpFragBuff.stStats.ulRxIpv6Frag);
    vos_printf("接收IPV6 IPF过滤成功的个数        %d\n",g_stCdsIpFragBuff.stStats.ulRxIpv6IpfSucc);
    vos_printf("接收IPV6 IPF过滤失败的个数        %d\n",g_stCdsIpFragBuff.stStats.ulRxIpv6IpfFail);
    vos_printf("接收本地处理成功的个数            %d\n",g_stCdsIpFragBuff.stStats.ulLocalProcSucc);
    vos_printf("接收本地处理失败的个数            %d\n",g_stCdsIpFragBuff.stStats.ulLocalProcFail);

    return;
}


VOS_VOID CDS_IpFrag_Help(VOS_VOID)
{
    vos_printf("CDS_ShowIpV4FragmentBuffInfo             显示IPV4分片缓存信息\n");
    vos_printf("CDS_ShowIpV6FragmentBuffInfo             显示IPV6分片缓存信息\n");
    vos_printf("CDS_ShowIpFragStats                      显示IP分片统计信息\n");
    vos_printf("CDS_ClearIpFragBuff                      清空IP分片缓存");

    return;
}


VOS_VOID CDS_Help(VOS_VOID)
{

    vos_printf("********************CDS软调信息************************\n");
    vos_printf("CDS_ShowCdsEntityInfo(ModemId)          显示对应Modem的CDS实体状态\n");
    vos_printf("CDS_ShowEventStats                      显示CDS事件处理统计信息\n");
    vos_printf("CDS_ShowUlRdProcStats                   显示CDS上行RD处理统计信息\n");
    vos_printf("CDS_ShowUlAdqProcStats                  显示CDS上行ADQ统计信息\n");
    vos_printf("CDS_ShowUlIpPktProcStats                显示CDS上行IP包处理统计信息\n");
    vos_printf("CDS_ShowDlSduProcStats                  显示CDS下行SDU数传统计信息\n");
    vos_printf("CDS_ShowDlIpfProcStats                  显示CDS下行IPF数传统计信息\n");

    vos_printf("CDS_ShowModemGUProcStats(ModemId)       显示GU上行数传统计信息\n");
    vos_printf("CDS_ShowLBProcStats                     显示环回模式数传统计信息\n");
    vos_printf("CDS_IpFrag_Help                         显示IP分片软调信息\n");
    vos_printf("QosFc_ShowStats                         显示流量控制数传统计信息\n");

    vos_printf("CDS_ShowUlSoftFilter(ModemId)           显示上行过滤器组信息\n");
    vos_printf("CDS_ShowUlSoftFilterStats               显示上行软过滤处理统计信息\n");
    vos_printf("CDS_ShowIMSStats                        显示IMS处理统计信息\n");

    return;
}
/*****************************************************************************
 函 数 名  : LUP_CdsLomTraceRcvUlData
 功能描述  : 用户面时延Trace，用于记录CDS接收上行数据时间戳
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月23日
    作    者   : hujianbo 42180
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID Cds_LomTraceRcvUlData(VOS_VOID)
{
    if (1 == g_ulCdsOmSwitch)
    {
        CDS_ERROR_LOG(UEPS_PID_CDS, "User plane latency trace: CDS rcv ul data\n");
    }

    return;
}

/*****************************************************************************
 函 数 名  : Cds_LomTraceSendDlData
 功能描述  : 用户面时延Trace，用于记录CDS发送下行数据时间戳
 输入参数  :
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2012年5月23日
    作    者   : hujianbo 42180
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID Cds_LomTraceSendDlData(VOS_VOID)
{
    if (1 == g_ulCdsOmSwitch)
    {
        CDS_ERROR_LOG(UEPS_PID_CDS, "User plane latency trace: CDS send dl data\n");
    }

    return;
}





#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



