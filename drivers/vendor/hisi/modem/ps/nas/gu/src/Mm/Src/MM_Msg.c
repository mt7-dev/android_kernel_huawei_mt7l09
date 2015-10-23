

#include        "MM_Inc.h"
#include        "GmmMmInterface.h"
#include        "GmmExt.h"

#include "NasNvInterface.h"
#include "TafNvInterface.h"

#include "NasUtranCtrlInterface.h"

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_MM_MSG_C



VOS_VOID MM_ReportN2MOtaMsg(VOS_UINT8 ucMsgType, NAS_MSG_STRU *pNasMsg)
{
    NAS_OTA_MSG_ID_ENUM_UINT16 usNasOtaMsyId = NAS_OTA_MSG_ID_BUTT;

    PS_LOG1(WUEPS_PID_MM, VOS_NULL, PS_PRINT_NORMAL, "MM_ReportN2MOtaMsg:NORMAL: msg type is ", ucMsgType);

    switch(ucMsgType)
    {
        case MM_IE_MSG_TYPE_AUTH_REQ:                                           /* AUTHENTICATION REQUEST 消息              */
            usNasOtaMsyId = NAS_OTA_MSG_AUTH_REQ;
            break;

        case MM_IE_MSG_TYPE_CM_SERV_REJ:                                        /* CM SERVICE REJECT 消息                   */
            usNasOtaMsyId = NAS_OTA_MSG_CM_SERV_REJECT;
            break;

        case MM_IE_MSG_TYPE_CM_SERV_ACP:                                        /* CM SERVICE ACCEPT 消息                   */
            usNasOtaMsyId = NAS_OTA_MSG_CM_SERV_ACCEPT;
            break;

        case MM_IE_MSG_TYPE_AUTH_REJ:                                           /* AUTHENTICATION REJECT 消息               */
            usNasOtaMsyId = NAS_OTA_MSG_AUTH_REJECT;
            break;

        case MM_IE_MSG_TYPE_ABORT:                                              /* ABORT 消息                               */
            usNasOtaMsyId = NAS_OTA_MSG_ABORT;
            break;

        case MM_IE_MSG_TYPE_ID_REQ:                                             /* IDENTITY REQUEST 消息                    */
            usNasOtaMsyId = NAS_OTA_MSG_ID_REQ;
            break;

        case MM_IE_MSG_TYPE_MM_INFO:                                            /* MM INFORMATION 消息                      */
            usNasOtaMsyId = NAS_OTA_MSG_MM_INFO;
            break;

        case MM_IE_MSG_TYPE_MM_STA:                                             /* MM STATUS 消息                           */
            usNasOtaMsyId = NAS_OTA_MSG_MM_STA_N2M;
            break;

        case MM_IE_MSG_TYPE_TMSI_REALLOC_CMD:                                   /* TMSI REALLOCATION COMMAND 消息           */
            usNasOtaMsyId = NAS_OTA_MSG_TMSI_REALLOC_CMD;
            break;

        case MM_IE_MSG_TYPE_LU_ACCEPT:
            usNasOtaMsyId = NAS_OTA_MSG_LOCATION_UPDATE_ACCEPT;
            break;

        case MM_IE_MSG_TYPE_LU_REJECT:
            usNasOtaMsyId = NAS_OTA_MSG_LOCATION_UPDATE_REJECT;
            break;

        default:
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "MM_ReportN2MOtaMsg:WARNING: invalid msg id");
            /*不属于MM模块的空口消息，不在trace中显示*/
            return;
    }

    NAS_SendAirMsgToOM(WUEPS_PID_MM, usNasOtaMsyId, NAS_OTA_DIRECTION_DOWN, pNasMsg->ulNasMsgSize + 4, (VOS_UINT8*)pNasMsg);
}


VOS_UINT8 Mm_ComMsgAuthReqRcv (
                            VOS_UINT8      *pucRcvMsg,                              /* 当前处理的消息                           */
                            VOS_UINT16      usMsgSize                               /* 消息长度,以字节为单位                    */
                            )
{
    VOS_UINT8       ucEventId = MM_EVENT_ID_AUTH_REQ;                               /* 事件ID                                   */
    VOS_UINT16      usIndex;

    PS_MEM_SET( &g_MmMsgAuthReq, 0, sizeof(MM_MSG_AUTHENTICATION_REQ_STRU));   /* 清零                                     */

    if (VOS_NULL_PTR == pucRcvMsg)
    {
        ucEventId = MM_EVENT_ID_INVALID;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgAuthReqRcv:WARNING: MSG VOS_NULL_PTR!");
    }
    else if ( usMsgSize < 19 )
    {
        ucEventId = MM_EVENT_ID_STATUS_96;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgAuthReqRcv:WARNING: RCV MM AUTHENTICATION REQ MSG WITH ERROR SIZE!");
    }
    else if (MM_FALSE == MM_IeChkCksn( &pucRcvMsg[2] ))
    {                                                                           /* CKSN为保留值                             */
        ucEventId = MM_EVENT_ID_STATUS_95;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgAuthReqRcv:WARNING: CKSN ERROR!");
    }
    else
    {
        g_MmMsgAuthReq.MmIeCKSN.ucCksn
            = (VOS_UINT8)(pucRcvMsg[2] & MM_IE_CKSN_KEY_SEQUENCE_MASK);         /* 存储CKSN                                 */

        NAS_MML_SetSimCsSecurityCksn(g_MmMsgAuthReq.MmIeCKSN.ucCksn);

        if ( MM_FALSE == MM_IeChkRand( &pucRcvMsg[3] ) )
        {
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgAuthReqRcv:WARNING: RAND ERROR!");
            return MM_EVENT_ID_STATUS_95;
        }
        PS_MEM_CPY(
                    g_MmMsgAuthReq.MmIeRAND.aucRandValue,
                    pucRcvMsg+3,
                    MM_RAND_VALUE_LENGTH
                    );                                                          /* 存储RAND                                 */
        usIndex = 19;
        while ( usIndex < usMsgSize )
        {                                                                       /* 消息里含有AUTN                           */
            if ( ( 0x20 == pucRcvMsg[usIndex] ) && ( usMsgSize > 36 ) )
            {                                                                   /* AUTN的IEI正确                            */
                usIndex++;
                if ( MM_FALSE == MM_IeChkAutn( &pucRcvMsg[usIndex] ) )
                {
                    PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgAuthReqRcv:WARNING: AUTN ERROR!");
                    return MM_EVENT_ID_STATUS_95;
                }
                if ( MM_IE_AUTN_PRESENT == g_MmMsgAuthReq.ucAutnFlg)
                {
                    ;
                }
                else
                {
                    g_MmMsgAuthReq.ucAutnFlg = MM_IE_AUTN_PRESENT;
                    g_MmMsgAuthReq.MmIeAUTN.ucAutnLen = pucRcvMsg[usIndex];     /* 获得AUTN的长度                           */
                    PS_MEM_CPY(
                                g_MmMsgAuthReq.MmIeAUTN.aucAutn,
                                &pucRcvMsg[usIndex + 1],
                                pucRcvMsg[usIndex]
                                );                                              /* 获得AUTN                                 */
                }
                usIndex = usIndex + pucRcvMsg[usIndex] + 1;
            }
            else
            {                                                                   /* AUTN的IEI不正确                          */
                usIndex = usMsgSize;
            }
        }
    }
    return ucEventId;                                                           /* 返回事件ID                               */
}

/*******************************************************************************
  Module:   Mm_ComMsgCcMsgInteChk
  Function: 对CC消息进行完整性检测
  Input:    RRMM_DATA_IND_STRU       *pMsg, RRMM_DATA_IND原语指针
  Output:
  NOTE:
  Return:   VOS_UINT8,事件ID,详见MM_Def.h中关于事件的宏定义
  History:
      1.  张志勇   2003.12.11  新版作成
      2. 日    期   : 2006年11月21日
         作    者   : luojian   id:60022475
         修改内容   : 去掉完整性检查判断,问题单号：A32D07459
*******************************************************************************/
VOS_UINT8 Mm_ComMsgCcMsgInteChk (
                              RRMM_DATA_IND_FOR_PCLINT_STRU       *pMsg         /* 当前处理的消息                           */
                               )
{
    VOS_UINT8       ucEventId;                                                  /* 事件ID                                   */
    VOS_UINT8       i;                                                          /* 循环变量                                 */
    VOS_UINT8       j;                                                          /* 循环变量                                 */
    VOS_UINT8       ucCnt = 0;                                                  /* MM连接的存在个数                         */

    for ( i = MM_CONST_NUM_0; i < MM_CONST_NUM_2; i++ )
    {
        for ( j = MM_CONST_NUM_0; j < MM_CONST_NUM_7; j++ )
        {
            if ( MM_CONST_NUM_0 !=
                ( g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].
                aucMMConnExtFlg[i] & (0x01 << j) ) )
            {                                                                   /* 统计MM连接的个数                         */
                ucCnt++;
            }
            if ( MM_CONST_NUM_0 !=
                ( g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SS].
                aucMMConnExtFlg[i] & (0x01 << j) ) )
            {                                                                   /* 统计MM连接的个数                         */
                ucCnt++;
            }
            if ( MM_CONST_NUM_0 !=
                ( g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SMS].
                aucMMConnExtFlg[i] & (0x01 << j) ) )
            {                                                                   /* 统计MM连接的个数                         */
                ucCnt++;
            }
        }
    }
    if ((MM_CONST_NUM_1 == ucCnt)
        &&(NO_MM_CONN_ESTING ==
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].ucMMConnEstingTI)
        &&(MM_TRUE ==
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].ucEstingCallTypeFlg))
    {                                                                           /* 如果MM连接只有一条并且是紧急呼           */
        ucEventId = MM_EVENT_ID_CC_MESSAGE;                                     /* 获得事件ID                               */
    }

    else
    {
        ucEventId = MM_EVENT_ID_CC_MESSAGE;                                     /* 获得事件ID                               */
    }
    return ucEventId;                                                           /* 返回事件ID                               */
}

/*******************************************************************************
  Module:   Mm_ComMsgCmSvcAcceptRcv
  Function: 对CM SERVICE ACCEPT消息进行完整性检测
  Input:    RRMM_DATA_IND_STRU       *pMsg, RRMM_DATA_IND原语指针
  Output:
  NOTE:
  Return:   VOS_UINT8,事件ID,详见MM_Def.h中关于事件的宏定义
  History:
      1.  张志勇    2003.12.11  新版作成
*******************************************************************************/
VOS_UINT8 Mm_ComMsgCmSvcAcceptRcv (
                                RRMM_DATA_IND_FOR_PCLINT_STRU       *pMsg       /* 当前处理的消息                           */
                                  )
{
    VOS_UINT8       ucEventId;                                                  /* 事件ID                                   */

    if ((MM_CONST_NUM_0 ==
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].aucMMConnExtFlg[0])
        &&(MM_CONST_NUM_0 ==
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].aucMMConnExtFlg[1])
        && (MM_CONST_NUM_0 ==
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SMS].aucMMConnExtFlg[0])
        && (MM_CONST_NUM_0 ==
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SMS].aucMMConnExtFlg[1])
        && (MM_CONST_NUM_0 ==
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SS].aucMMConnExtFlg[0])
        && (MM_CONST_NUM_0 ==
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SS].aucMMConnExtFlg[1])
        &&(NO_MM_CONN_ESTING !=
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].ucMMConnEstingTI)
        &&(MM_TRUE ==
        g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].ucEstingCallTypeFlg))
    {
        ucEventId = MM_EVENT_ID_CM_SVC_ACCEPT;                                  /* 获得事件ID                               */
    }
    else
    {
        ucEventId = MM_EVENT_ID_CM_SVC_ACCEPT;                                  /* 获得事件ID                               */
    }

    Mm_TimerStop(MM_TIMER_T3218);                                               /* 停T3218                                  */
    Mm_ComDelRandRes();                                                         /* 删除RAND和RES                            */

    return ucEventId;                                                           /* 返回事件ID                               */
}
VOS_UINT8 Mm_ComMsgAuthRjctRcv ()
{
    VOS_UINT8       ucEventId = MM_EVENT_ID_AUTH_REJ;                               /* 事件ID                                   */
	NAS_MML_PLMN_ID_STRU *         pstCurrCampPlmnId;
	NAS_MML_IGNORE_AUTH_REJ_INFO_STRU      *pstAuthRejInfo;
    g_MmGlobalInfo.LuInfo.ucT3212ExpiredFlg = MM_FALSE;
    g_MmGlobalInfo.LuInfo.ucLuType = MM_IE_LUT_TYPE_BUTT;

    pstCurrCampPlmnId = NAS_MML_GetCurrCampPlmnId();
    pstAuthRejInfo = NAS_MML_GetAuthRejInfo();
  
    if (pstAuthRejInfo->ucHplmnCsAuthRejCounter < pstAuthRejInfo->ucMaxAuthRejNo)
    {
        if ((VOS_TRUE == NAS_MML_ComparePlmnIdWithHplmn(pstCurrCampPlmnId))
        && (VOS_TRUE == pstAuthRejInfo->ucIgnoreAuthRejFlg))
        {
            pstAuthRejInfo->ucHplmnCsAuthRejCounter++;
            return MM_EVENT_ID_INVALID;
        }            
    } 

    return ucEventId;                                                           /* 返回事件ID                               */
}

/*******************************************************************************
  Module:   Mm_ComMsgCmSvcRjctRcv
  Function: 存储消息CM SERVICE REJECT
  Input:    VOS_UINT8    *pucRcvMsg, 当前处理的消息
  Output:
  NOTE:
  Return:   VOS_UINT8,事件ID,详见MM_Def.h中关于事件的宏定义
  History:
      1.  张志勇   2003.12.10   新版作成
*******************************************************************************/
VOS_UINT8 Mm_ComMsgCmSvcRjctRcv (
                            VOS_UINT8      *pucRcvMsg,                          /* 当前处理的消息                           */
                            VOS_UINT16      usMsgSize                           /* 消息长度,以字节为单位                    */
                            )
{
    VOS_UINT8       ucEventId = MM_EVENT_ID_CM_SVC_REJ;                         /* 事件ID                                   */

    if ( usMsgSize < 3 )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgCmSvcRjctRcv:WARNING: RCV MM CM SERVICE REJECT MSG WITH ERROR SIZE!");
        return MM_EVENT_ID_STATUS_96;
    }
    g_MmMsgCmSvcRjct.MmIeRejCause.ucRejCause = pucRcvMsg[2];                    /* 存储Reject cause                         */

    g_MmGlobalInfo.ucRejectCause = pucRcvMsg[2];

    Mm_TimerStop(MM_TIMER_T3218);                                               /* 停T3218                                  */
    Mm_ComDelRandRes();                                                         /* 删除RAND和RES                            */

    return ucEventId;                                                           /* 返回事件ID                               */
}

/*******************************************************************************
  Module:   Mm_ComMsgAbortRcv
  Function: 存储消息ABORT
  Input:    VOS_UINT8    *pucRcvMsg, 当前处理的消息
  Output:
  NOTE:
  Return:   VOS_UINT8,事件ID,详见MM_Def.h中关于事件的宏定义
  History:
      1.  张志勇   2003.12.10   新版作成
*******************************************************************************/
VOS_UINT8 Mm_ComMsgAbortRcv (
                            VOS_UINT8      *pucRcvMsg,                          /* 当前处理的消息                           */
                            VOS_UINT16      usMsgSize                           /* 消息长度,以字节为单位                    */
                            )
{
    VOS_UINT8       ucEventId = MM_EVENT_ID_ABORT;                              /* 事件ID                                   */

    if ( usMsgSize < 3 )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgAbortRcv:WARNING: RCV MM ABORT MSG WITH ERROR SIZE!");
        return MM_EVENT_ID_STATUS_96;
    }
    g_MmMsgAbort.MmIeRejCause.ucRejCause = pucRcvMsg[2];                        /* 存储Reject cause                         */

    g_MmGlobalInfo.ucRejectCause = pucRcvMsg[2];

    return ucEventId;                                                           /* 返回事件ID                               */
}

/*******************************************************************************
  Module:   Mm_ComMsgIdReqRcv
  Function: 存储消息 IDENTITY REQUEST
  Input:    VOS_UINT8    *pucRcvMsg, 当前处理的消息
  Output:
  NOTE:
  Return:   VOS_UINT8,事件ID,详见MM_Def.h中关于事件的宏定义
  History:
      1.  张志勇   2003.12.10   新版作成

      2.日    期   : 2006年2月20日
        作    者   : liuyang id:48197
        修改内容   : 将IE错误的上报的原因值由95改为96,问题单号:A32D01694
*******************************************************************************/
VOS_UINT8 Mm_ComMsgIdReqRcv (
                            VOS_UINT8      *pucRcvMsg,                              /* 当前处理的消息                           */
                            VOS_UINT16      usMsgSize                               /* 消息长度,以字节为单位                    */
                            )
{
    VOS_UINT8       ucEventId = MM_EVENT_ID_IDENTITY_REQ;                           /* 事件ID                                   */

    if ( usMsgSize < 3 )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgIdReqRcv:WARNING: RCV MM IDENTITY REQUEST MSG WITH ERROR SIZE!");
        return MM_EVENT_ID_STATUS_96;
    }

    if ( MM_FALSE == MM_IeChkIdType( &pucRcvMsg[2] ) )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgIdReqRcv:WARNING: ID TYPE ERROR!");
        return MM_EVENT_ID_STATUS_96;
    }
    g_MmMsgIdReq.MmIeIdType.ucIDType
        = (VOS_UINT8)(pucRcvMsg[2] & MM_IE_ID_TYPE_IDTYPE_MASK);                    /* 存储Id type                              */
    return ucEventId;                                                           /* 返回事件ID                               */
}
VOS_UINT8 Mm_ComMsgMmInfoRcv (
    VOS_UINT8                           *pucRcvMsg,
    VOS_UINT16                          usMsgSize
)
{
    VOS_UINT8                           ucEventId;
    VOS_UINT16                          usIndex;
    VOS_UINT8                           ucShortNetWorkNameExist;
    VOS_UINT8                           ucLongNetWorkNameExist;
    MM_DECODE_RESULT_ENUM_U32           ucRst;
    NAS_MML_OPERATOR_NAME_INFO_STRU     stOldOperatorName;
    NAS_MML_OPERATOR_NAME_INFO_STRU    *pstCurrOperatorName = VOS_NULL_PTR;

    NAS_MMC_NVIM_OPERATOR_NAME_INFO_STRU                    stNvimOperatorName;

    ucShortNetWorkNameExist = VOS_FALSE;
    ucLongNetWorkNameExist  = VOS_FALSE;
    ucEventId               = MM_EVENT_ID_MM_INFO;
    ucRst                   = MM_DECODE_SUCCESS;

    pstCurrOperatorName = NAS_MML_GetOperatorNameInfo();
    PS_MEM_CPY(&stOldOperatorName, pstCurrOperatorName, sizeof(NAS_MML_OPERATOR_NAME_INFO_STRU));

    PS_MEM_SET(&g_MmMsgMmInfo, 0, sizeof(MM_MSG_MM_INFO_STRU));

    if ( usMsgSize < MM_MSG_HEADER_LEN )
    {
        NAS_WARNING_LOG(WUEPS_PID_MM, "Mm_ComMsgMmInfoRcv:WARNING: RCV MM INFORMATION MSG WITH ERROR SIZE!");
        return MM_EVENT_ID_STATUS_96;
    }

    usIndex = MM_MSG_HEADER_LEN;
    while ( usIndex < usMsgSize )
    {
        switch (pucRcvMsg[usIndex])
        {
            case MM_IE_FULL_NAME_FOR_NETWORK:
                ucRst = NAS_MM_DecodeFullNameForNetworkIe(pucRcvMsg, usMsgSize,
                                                          &usIndex, &ucEventId,
                                                          &ucLongNetWorkNameExist);


                g_MmMsgMmInfo.ucFulNamFrNWFlg = VOS_TRUE;

                break;
            case MM_IE_SHORT_NAME_FOR_NETWORK:
                ucRst = NAS_MM_DecodeShortNameForNetworkIe(pucRcvMsg, usMsgSize,
                                                          &usIndex, &ucEventId,
                                                          &ucShortNetWorkNameExist);

                g_MmMsgMmInfo.ucShortNamFrNWFlg = VOS_TRUE;

                break;
            case MM_IE_LOCAL_TIME_ZONE:
                ucRst = NAS_MM_DecodeLocalTimeZoneIe(pucRcvMsg, usMsgSize,
                                             &usIndex, &ucEventId);
                break;
            case MM_IE_UNIVERSAL_TIME_AND_LOCAL_TIME_ZONE:
                ucRst = NAS_MM_DecodeUniversalTimeAndLocalTimeZoneIe(pucRcvMsg, usMsgSize,
                                                     &usIndex, &ucEventId);
                break;
            case MM_IE_LSA_IDENTITY:
                ucRst = NAS_MM_DecodeLocalizedServiceAreaIe(pucRcvMsg, usMsgSize,
                                                     &usIndex, &ucEventId);
                break;
            case MM_IE_NETWORK_DAYLIGHT_SAVING_TIME:
                ucRst = NAS_MM_DecodeNetworkDaylightSavingTimeIe(pucRcvMsg, usMsgSize,
                                                     &usIndex, &ucEventId);
                break;
            default:
                usIndex = usMsgSize;
                break;
        }

        /* 如果消息解析后的网络名与之前保存的内容不同,需要写NV */
        if (0 != VOS_MemCmp(pstCurrOperatorName, &stOldOperatorName, sizeof(NAS_MML_OPERATOR_NAME_INFO_STRU)))
        {
            PS_MEM_CPY(&stNvimOperatorName, pstCurrOperatorName, sizeof(stNvimOperatorName));

            if (NV_OK != NV_Write(en_NV_Item_Network_Name_From_MM_Info,
                                  &stNvimOperatorName,
                                  sizeof(stNvimOperatorName)))
            {
                NAS_WARNING_LOG(WUEPS_PID_MM, "Mm_ComMsgMmInfoRcv:Write NV fail.");
            }
        }

        if(MM_DECODE_END == ucRst)
        {
            return ucEventId;
        }
    }

    return ucEventId;                                                           /* 返回事件ID                               */
}

/*******************************************************************************
  Module:   Mm_ComMsgMmStatusRcv
  Function: 存储消息MM STATUS
  Input:    VOS_UINT8    *pucRcvMsg, 当前处理的消息
  Output:
  NOTE:
  Return:   VOS_UINT8,事件ID,详见MM_Def.h中关于事件的宏定义
  History:
      1.  张志勇   2003.12.10   新版作成
*******************************************************************************/
VOS_UINT8 Mm_ComMsgMmStatusRcv (
                            VOS_UINT8      *pucRcvMsg,                          /* 当前处理的消息                           */
                            VOS_UINT16      usMsgSize                           /* 消息长度,以字节为单位                    */
                            )
{
    VOS_UINT8       ucEventId = MM_EVENT_ID_MM_STATUS;                          /* 事件ID                                   */

    if ( usMsgSize < 3 )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgMmStatusRcv:WARNING: RCV MM STATUS MSG WITH ERROR SIZE!");
        return MM_EVENT_ID_STATUS_96;
    }

    g_MmGlobalInfo.ucRejectCause = pucRcvMsg[2];

    return ucEventId;                                                           /* 返回事件ID                               */
}



VOS_VOID Mm_ComSaveLai(
                   VOS_UINT8            *pucLai,                                    /* 消息中的LAI存放地址                      */
                   MM_LAI_STRU      *pLai                                       /* LAI全局量的地址                          */
                   )
{
    pLai->PlmnId.ulMcc = pucLai[1] & MM_IE_LU_ID_MCC_DIGIT_3_MASK;              /* 存储LAI中的 MCC digit 3                  */
    pLai->PlmnId.ulMcc = (pLai->PlmnId.ulMcc << 8) | (pucLai[0] >> 4);          /* 存储LAI中的 MCC digit 2                  */
    pLai->PlmnId.ulMcc = (pLai->PlmnId.ulMcc << 8)
                         | (pucLai[0] & MM_IE_LU_ID_MCC_DIGIT_1_MASK);          /* 存储LAI中的 MCC digit 1                  */

    pLai->PlmnId.ulMnc = pucLai[1] >> 4;                                        /* 存储LAI中的 MNC digit 3                  */
    pLai->PlmnId.ulMnc = (pLai->PlmnId.ulMnc << 8) | (pucLai[2] >> 4);          /* 存储LAI中的 MNC digit 2                  */
    pLai->PlmnId.ulMnc = (pLai->PlmnId.ulMnc << 8)
                         | (pucLai[2] & MM_IE_LU_ID_MNC_DIGIT_1_MASK);          /* 存储LAI中的 MNC digit 1                  */
    pLai->ulLac = pucLai[3];
    pLai->ulLac = (pLai->ulLac << 8) | pucLai[4];

    if ( 0 == ( pLai->ulLac & 0x0000ffff ) )
    {
        pLai->ulLac = MM_INVALID_LAI;
    }

}


VOS_UINT8 Mm_ComMsgTmsiReallocCmdRcv (
                            VOS_UINT8      *pucRcvMsg,                              /* 当前处理的消息                           */
                            VOS_UINT16      usMsgSize                               /* 消息长度,以字节为单位                    */
                            )
{
    VOS_UINT8       ucEventId = MM_EVENT_ID_TMSI_REALLOC_CMD;                       /* 事件ID                                   */
    VOS_UINT8       i;                                                              /* 循环变量                                 */

    if ( usMsgSize < 9 )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgTmsiReallocCmdRcv:WARNING: RCV TMSI REALLOCATION COMMAND MSG WITH ERROR SIZE!");
        return MM_EVENT_ID_STATUS_96;
    }
    PS_MEM_SET(&g_MmMsgTmsiReallocCmd,0,sizeof(MM_MSG_TMSI_REALLOC_CMD_STRU)); /* 清零                                     */

    if ( MM_FALSE == MM_IeChkLai( &pucRcvMsg[2] ) )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgTmsiReallocCmdRcv:WARNING: LAI ERROR!");
    }
    Mm_ComSaveLai (pucRcvMsg + 2, &g_MmMsgTmsiReallocCmd.MmIeLAI.IeLai);        /* 存储消息中的LAI                          */

    if ( ( 0xF4 != pucRcvMsg[8] ) && ( 0x01 != (pucRcvMsg[8] & 0x0F) )
                                && ( 0x09 != (pucRcvMsg[8] & 0x0F) ) )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgTmsiReallocCmdRcv:WARNING: MOBILE ID TYPE ERROR!");
        return MM_EVENT_ID_STATUS_95;
    }
    if ( MM_FALSE == MM_IeChkMblId( &pucRcvMsg[7] ) )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgTmsiReallocCmdRcv:WARNING: MOBILE ID ERROR!");
        return MM_EVENT_ID_STATUS_95;
    }
    switch ( pucRcvMsg[8] & MM_IE_MI_ID_TYPE_MASK )
    {                                                                           /* 判断MOBILE ID的类型                      */
    case MM_IE_MI_ID_TYPE_IMSI:                                                 /* IMSI                                     */
        g_MmMsgTmsiReallocCmd.MmIeMobileId.MobileID.ucMsIdFlg
            = MM_MS_ID_IMSI_PRESENT;                                            /* 置Mobile ID存在标志                      */
        break;
    case MM_IE_MI_ID_TYPE_TMSI:                                                 /* TMSI                                     */
        g_MmMsgTmsiReallocCmd.MmIeMobileId.MobileID.ucMsIdFlg
            = MM_MS_ID_TMSI_PRESENT;                                            /* 置Mobile ID存在标志                      */
        for ( i = MM_CONST_NUM_0; i < MM_CONST_NUM_4; i++ )
        {
            g_MmMsgTmsiReallocCmd.MmIeMobileId.MobileID.aucTmsi[i]
                = pucRcvMsg[9+i];                                               /* 获得TMSI                                 */
        }
        break;
    default:
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgTmsiReallocCmdRcv:WARNING: MOBILE IDENTITY TYPE Abnormal");
        break;
    }
    return ucEventId;                                                           /* 返回事件ID                               */
}
VOS_UINT32  MM_ComCheckNoOtherMmConnection()
{
    if ( (MM_CONST_NUM_0 ==  g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SMS].aucMMConnExtFlg[0])
      && (MM_CONST_NUM_0 ==  g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SMS].aucMMConnExtFlg[1])
      && (MM_CONST_NUM_0 ==  g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SS].aucMMConnExtFlg[0])
      && (MM_CONST_NUM_0 ==  g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_SS].aucMMConnExtFlg[1]))
    {
        return MM_TRUE;
    }
    else
    {
        return MM_FALSE;
    }
}


VOS_UINT32 MM_ComCmpLai(MM_LAI_STRU stLai1,MM_LAI_STRU stLai2)
{
    if ((stLai1.ulLac != stLai2.ulLac)
     || (stLai1.PlmnId.ulMnc != stLai2.PlmnId.ulMnc)
     || (stLai1.PlmnId.ulMcc != stLai2.PlmnId.ulMcc))
    {
        return MM_FALSE;
    }
    else
    {
        return MM_TRUE;
    }
}


VOS_UINT32 MM_ComCheckLauIntegrityProtection(VOS_UINT8 *pucMsgContent, VOS_UINT16  usMsgLen)
{
    MM_LAI_STRU                         stLuLai;
    VOS_UINT8                           aucLuTmsi[MM_TMSI_LENGTH];
    VOS_UINT8                           ucTmsiAvail;
    VOS_UINT8                           ucImsiAvail;
    VOS_UINT16                          usIeCount;
    VOS_UINT32                          ulIntegrityCheckResult;

    /* 完整性保护已经启动 */
    if (NAS_MML_RRC_INTEGRITY_PROTECT_ACTIVE == g_MmSubLyrShare.MmShare.ucCsIntegrityProtect)
    {
        ulIntegrityCheckResult = MM_TRUE;
    }
    /* 完整性保护没有启动 */
    else
    {
        ulIntegrityCheckResult = MM_FALSE;

        /* 24.008 4.1.1.1  LOCATION UPDATING ACCEPT (at periodic location update with no change
        of location area or temporary identity) */
        if (MM_IE_LUT_PERIODIC_UPDATING != g_MmGlobalInfo.LuInfo.ucLuType)
        {
            return MM_FALSE;
        }

        /* 获取LAU ACCEPT消息LAI参数 */
        Mm_ComSaveLai( &pucMsgContent[2], &stLuLai );

        ucTmsiAvail = MM_FALSE;
        ucImsiAvail = MM_FALSE;
        PS_MEM_SET(aucLuTmsi, 0x00, MM_TMSI_LENGTH);

        /* 获取LAU ACCEPT消息TMSI参数 */
        for (usIeCount=7; usIeCount < usMsgLen; usIeCount++ )
        {
            if (MM_IEI_MOBILE_ID == pucMsgContent[usIeCount])
            {
                if ( MM_IE_MI_ID_TYPE_TMSI == (pucMsgContent[usIeCount+2] & 0x07) )
                {
                    ucTmsiAvail = MM_TRUE;
                    PS_MEM_CPY(aucLuTmsi, &pucMsgContent[usIeCount+3], MM_TMSI_LENGTH);
                }
                else if ( MM_IE_MI_ID_TYPE_IMSI == (pucMsgContent[usIeCount+2] & 0x07) )
                {
                    ucImsiAvail = MM_TRUE;
                }
                else
                {
                }
                break;
            }
        }

        /* LAI没有发生改变 */
        if (MM_TRUE == MM_ComCmpLai(stLuLai,g_MmGlobalInfo.MsCsInfo.CurLai))
        {
            /* 没有分配TMSI */
            if (MM_FALSE == ucTmsiAvail)
            {
                if (!( (MM_TRUE == ucImsiAvail)
                      &&( MM_MS_ID_TMSI_PRESENT == ( g_MmGlobalInfo.MsCsInfo.MobileId.ucMsIdFlg
                                                   & MM_MS_ID_TMSI_PRESENT ) ))
                   )
                {
                    ulIntegrityCheckResult = MM_TRUE;
                }
            }
            /* 分配的TMSI和以前相同 */
            else if ( 0 == VOS_MemCmp(aucLuTmsi,NAS_MML_GetUeIdTmsi(),MM_TMSI_LENGTH) )
            {
                ulIntegrityCheckResult = MM_TRUE;
            }
            else
            {
            }
        }
    }
    return ulIntegrityCheckResult;
}
VOS_UINT32 MM_ComCheckIntegrityProtection(VOS_UINT8 *pucMsgContent, VOS_UINT16  usMsgLen)
{
    VOS_UINT8                           ucPD;
    VOS_UINT8                           ucMsgType;
    VOS_UINT32                          ulIntegrityCheckResult;

    /* 快速接入模式属于测试模式，不进行完整性检查 */
    if ( VOS_TRUE == NAS_MML_IsQuickStartFlg())
    {
        return MM_TRUE;
    }

    /* 完整性检查仅适用于UMTS网络 */
    if (NAS_MML_NET_RAT_TYPE_WCDMA != NAS_MML_GetCurrNetRatType())
    {
        return MM_TRUE;
    }


    /* 在TDD模式时候，如果配置不需要进行SMC设置，则返回完整性保护通过 */
    if (VOS_FALSE == NAS_UTRANCTRL_IsUtranCsSmcNeeded())
    {
        return MM_TRUE;
    }

    ucPD = pucMsgContent[0] & 0x0f;

    /* 完整性保护已经启动 */
    if (NAS_MML_RRC_INTEGRITY_PROTECT_ACTIVE == g_MmSubLyrShare.MmShare.ucCsIntegrityProtect)
    {
        ulIntegrityCheckResult = MM_TRUE;
    }
    /* 完整性保护没有启动 */
    else
    {
        ulIntegrityCheckResult = MM_FALSE;

        /*
        24.008 4.1.1.1 Except the messages listed below, no layer 3 signalling
        messages shall be processed by the receiving MM and GMM entities or
        forwarded to the CM entities, unless the security mode control procedure
        is activated for that domain. */
        if (MM_IE_PD_MM_MSG == ucPD)
        {
            ucMsgType = pucMsgContent[1] & 0x3f;
            switch (ucMsgType)
            {
                case MM_IE_MSG_TYPE_AUTH_REQ:
                case MM_IE_MSG_TYPE_AUTH_REJ:
                case MM_IE_MSG_TYPE_ID_REQ:
                case MM_IE_MSG_TYPE_LU_REJECT:
                case MM_IE_MSG_TYPE_CM_SERV_REJ:
                case MM_IE_MSG_TYPE_ABORT:
                    ulIntegrityCheckResult = MM_TRUE;
                    break;
                case MM_IE_MSG_TYPE_LU_ACCEPT:
                    ulIntegrityCheckResult = MM_ComCheckLauIntegrityProtection(pucMsgContent,usMsgLen);
                    break;

                case MM_IE_MSG_TYPE_CM_SERV_ACP:
                    if ( (MM_TRUE == MM_ComCheckNoOtherMmConnection())
                      && (WAIT_FOR_OUTGOING_MM_CONNECTION == g_MmGlobalInfo.ucState)
                      && (MMCC_EMERGENCY_CALL == g_MmCcEstReq.ulCallType) )
                    {
                        ulIntegrityCheckResult = MM_TRUE;
                    }
                    break;
                default:
                    break;
            }
        }
        else if(MM_IE_PD_CALL_CONTROL == ucPD)
        {
            /* 连接态没有其他MM连接，当前为紧急呼叫状态 */
            if ((MM_TRUE == MM_ComCheckNoOtherMmConnection())
             && (MM_CONNECTION_ACTIVE == g_MmGlobalInfo.ucState)
             && (MMCC_EMERGENCY_CALL == g_MmCcEstReq.ulCallType) )
            {
                ulIntegrityCheckResult = MM_TRUE;
            }
        }
        else if (MM_IE_PD_TEST == ucPD)
        {
            ulIntegrityCheckResult = MM_TRUE;
        }
        else
        {
        }
    }
    return ulIntegrityCheckResult;
}
VOS_VOID NAS_MM_SysInfoPreCheckClearAttemptCounter(VOS_VOID *pRcvMsg)
{
    MMCMM_GSM_SYS_INFO_IND_ST           *pSysInfoInd;
    MMCMM_SYS_INFO_IND_STRU             *pWSysInfoInd;
    MM_LAI_STRU                         stLai;

    pSysInfoInd = (MMCMM_GSM_SYS_INFO_IND_ST *)pRcvMsg;

    stLai.PlmnId.ulMcc = pSysInfoInd->stPlmnId.ulMcc;
    stLai.PlmnId.ulMnc = pSysInfoInd->stPlmnId.ulMnc;
    stLai.ulLac        = pSysInfoInd->usLac;
    if (NAS_MML_NET_RAT_TYPE_WCDMA == NAS_MML_GetCurrNetRatType())
    {
        pWSysInfoInd = (MMCMM_SYS_INFO_IND_STRU *)pRcvMsg;
        stLai.PlmnId = pWSysInfoInd->PlmnId;
        stLai.ulLac  = pWSysInfoInd->usLac;
    }

    /* 根据24.008 4.4.4.5 The attempt counter is reset when:
        in case of service state ATTEMPTING to UPDATE，
        a MS detects that a new location area is entered */
    if ((MM_IDLE_PLMN_SEARCH == g_MmGlobalInfo.ucState)
       && ((MM_IDLE_ATTEMPTING_TO_UPDATE == g_MmGlobalInfo.ucPreState)
       || (MM_IDLE_NO_CELL_AVAILABLE == g_MmGlobalInfo.ucPreState)))
    {
        if (   (stLai.PlmnId.ulMcc != g_MmGlobalInfo.MsCsInfo.CurLai.PlmnId.ulMcc)
            || (stLai.PlmnId.ulMnc != g_MmGlobalInfo.MsCsInfo.CurLai.PlmnId.ulMnc)
            || (stLai.ulLac        != g_MmGlobalInfo.MsCsInfo.CurLai.ulLac))
        {
            g_MmGlobalInfo.LuInfo.ucLuAttmptCnt = 0;
        }
    }
}

VOS_BOOL NAS_MM_IsIdleState()
{
    if ( (MM_IDLE_NO_CELL_AVAILABLE == g_MmGlobalInfo.ucState) ||
        (MM_IDLE_PLMN_SEARCH == g_MmGlobalInfo.ucState) ||
        (MM_IDLE_NORMAL_SERVICE == g_MmGlobalInfo.ucState) ||
        (MM_IDLE_LIMITED_SERVICE == g_MmGlobalInfo.ucState) ||
        (MM_IDLE_ATTEMPTING_TO_UPDATE == g_MmGlobalInfo.ucState) ||
        (MM_IDLE_LOCATION_UPDATE_NEEDED == g_MmGlobalInfo.ucState) ||
        (MM_IDLE_PLMN_SEARCH_NORMAL_SERVICE == g_MmGlobalInfo.ucState) ||
        (MM_IDLE_NO_IMSI == g_MmGlobalInfo.ucState) ||
        (LOCATION_UPDATING_PENDING == g_MmGlobalInfo.ucState))
    {
        return  VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_VOID NAS_MM_RcvSysInfoAccessBar_ProcBufferedCsService(VOS_VOID)
{
#if (FEATURE_ON == FEATURE_LTE)
    VOS_UINT8                                               ucCsRestrictPagingFlg;
    VOS_UINT8                                               ucRestrictNormalService;
    NAS_MM_COM_SERVICE_STATUS_ENUM_UINT8                    enServiceStatus;
    NAS_MML_CSFB_SERVICE_STATUS_ENUM_UINT8                  enCsfbServiceStatus;

    ucCsRestrictPagingFlg = VOS_FALSE;
#endif

    /* access bar时打紧急呼叫直接发起 */
    /* 判断紧急呼叫，不能光用RcvXXEstReq.ulCallType判断，前提条件是CC业务*/
    if (MM_CS_SERV_EMERGENCY_CALL_EXIST == Mm_ComJudgeCsServExist())
    {
        g_MmGlobalInfo.ucStaOfRcvXXEstReq = g_MmGlobalInfo.ucState; /* 记录收到建立请求时的状态                 */
        NAS_MM_EstMmConnection(MM_CONN_CTRL_CC);
        return;
    }

#if (FEATURE_ON == FEATURE_LTE)
    ucCsRestrictPagingFlg = NAS_MML_GetCsRestrictPagingFlg();
    enServiceStatus       = NAS_MM_GetLauUptNeededCsSrvStatus();
    enCsfbServiceStatus   = NAS_MML_GetCsfbServiceStatus();

    if ((NAS_MML_CSFB_SERVICE_STATUS_MT_EXIST == enCsfbServiceStatus)
     && (VOS_FALSE == ucCsRestrictPagingFlg)
     && ((MM_COM_SRVST_NORMAL_SERVICE == enServiceStatus)
      || (MM_IMSI_PAGING == g_MmGlobalInfo.ucPagingRecordTypeId)))
    {
        g_MmGlobalInfo.ucStaOfRcvXXEstReq = NAS_MM_GetStaOfRcvXXEstReq_CSFB();

        Mm_ComSetMmState(NAS_MM_GetStaOfRcvXXEstReq_CSFB());

        /* 在GU下，回复paging response */
        NAS_MM_ResponseLmmPagingInd_CSFB();

        /* 记录delay lau标识，呼叫结束后收到系统消息需要做lau */
        NAS_MML_SetDelayedCsfbLauFlg(VOS_TRUE);
        return;
    }

    ucRestrictNormalService = NAS_MML_GetCsRestrictNormalServiceFlg();

    if ((MM_COM_SRVST_NORMAL_SERVICE == enServiceStatus)
     && (VOS_FALSE == ucRestrictNormalService))
    {
        if (NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_CC_EXIST == enCsfbServiceStatus)
        {
            /* 记录delay lau标识，呼叫结束后收到系统消息需要做lau */
            NAS_MML_SetDelayedCsfbLauFlg(VOS_TRUE);

            g_MmGlobalInfo.ucStaOfRcvXXEstReq = NAS_MM_GetStaOfRcvXXEstReq_CSFB();

            Mm_ComSetMmState(NAS_MM_GetStaOfRcvXXEstReq_CSFB());

            NAS_MM_EstMmConnection(MM_CONN_CTRL_CC);
        }
        else if (NAS_MML_CSFB_SERVICE_STATUS_MO_NORMAL_SS_EXIST == enCsfbServiceStatus)
        {
            /* 记录delay lau标识，呼叫结束后收到系统消息需要做lau */
            NAS_MML_SetDelayedCsfbLauFlg(VOS_TRUE);

            g_MmGlobalInfo.ucStaOfRcvXXEstReq = NAS_MM_GetStaOfRcvXXEstReq_CSFB(); /* 记录收到建立请求时的状态                 */

            Mm_ComSetMmState(NAS_MM_GetStaOfRcvXXEstReq_CSFB());

            NAS_MM_PerformBufferedSsService();
        }
        else
        {
        }
    }
#endif

    return;
}



VOS_UINT8 Mm_SysInfo_Pre_Check(VOS_VOID *pRcvMsg)
{
    VOS_UINT8 ucEventID = 5;

    MMCMM_SYS_INFO_IND_STRU                *pSysInfoInd = VOS_NULL_PTR;                    /* 定义接收到的消息类型指针                 */


    MMCMM_GSM_SYS_INFO_IND_ST              *pGSysInfo = VOS_NULL_PTR;
    VOS_UINT8                               ucCsForbFlg;

    ucCsForbFlg = VOS_TRUE;

    if (NAS_MML_NET_RAT_TYPE_GSM == NAS_MML_GetCurrNetRatType())
    {
        pGSysInfo = (MMCMM_GSM_SYS_INFO_IND_ST *)pRcvMsg;
        if ((MMCMM_NO_FORBIDDEN            == pGSysInfo->ulForbiddenFlg)
         || (MMCMM_FORBIDDEN_PLMN_FOR_GPRS == pGSysInfo->ulForbiddenFlg))
        {
            ucCsForbFlg = VOS_FALSE;
        }
    }
    else
    {
        pSysInfoInd = (MMCMM_SYS_INFO_IND_STRU *)pRcvMsg;                                                      /* 消息转换                                */
        if ((MMCMM_NO_FORBIDDEN            == pSysInfoInd->ulForbiddenFlg)
         || (MMCMM_FORBIDDEN_PLMN_FOR_GPRS == pSysInfoInd->ulForbiddenFlg))
        {
            ucCsForbFlg = VOS_FALSE;
        }
    }

    /* MM在TC状态不处理来自MMC的系统消息。
       GMM中在Gmm_MsgDistribute_GMM_TC_ACTIVE 中已做了限制 */
    if (TEST_CONTROL_ACTIVE == g_MmGlobalInfo.ucState)
    {
        return MM_EVENT_ID_INVALID;
    }

    if ( MM_TIMER_RUNNING == NAS_MM_QryTimerStatus(MM_TIMER_HO_WAIT_SYSINFO) )
    {
        Mm_TimerStop(MM_TIMER_HO_WAIT_SYSINFO);
    }

    if ( MM_TIMER_RUNNING == NAS_MM_QryTimerStatus(MM_TIMER_EMERGENCY_CSFB_HO_WAIT_SYSINFO) )
    {
        Mm_TimerStop(MM_TIMER_EMERGENCY_CSFB_HO_WAIT_SYSINFO);
    }

    
    /* 若卡无效，假流程回复给MMC,在搜网注册时，若不回复，MMC会一直处在等待CS注册结果状态 */
    if (VOS_FALSE == NAS_MML_GetSimCsRegStatus())
    {
        NAS_MM_SndMmcCsRegResultInd(MM_MMC_LU_PROC_FALSE,
                                    MM_MMC_LU_RESULT_FAILURE,
                                    NAS_MML_REG_FAIL_CAUSE_SIM_INVALID);
        NAS_MM_SndGmmLuResultInd(MMGMM_LU_FAILURE,
                                 NAS_MML_REG_FAIL_CAUSE_ILLEGAL_MS);
        /* 非空闲态,不处理是否被bar */
        if ( VOS_TRUE == NAS_MM_IsIdleState() )
        {
            g_MmGlobalInfo.ucMmServiceState = MM_NO_IMSI;
            g_MmGlobalInfo.ucPreState = g_MmGlobalInfo.ucState;
            Mm_ComSetMmState(MM_IDLE_NO_IMSI);
        }

        /* 更新SYSINFO全局变量 */
        Mm_RcvMmcSysInfoInd(pRcvMsg);
        Mm_ComCheckDelayMmConn(MM_FALSE);

        ucEventID   = MM_EVENT_ID_INVALID;
    }
    else
    {
        if ((WAIT_FOR_RR_CONNECTION_MM_CONNECTION == g_MmGlobalInfo.ucState)
            && (MMCC_EMERGENCY_CALL == g_MmCcEstReq.ulCallType))
        {
            return ucEventID;
        }

        /* 注册受限且非forbidden的情况,forbidden的情况进入状态机处理 */
        if ((VOS_TRUE == NAS_MML_GetCsRestrictRegisterFlg())
         && (VOS_FALSE == ucCsForbFlg))
        {
            /* 非空闲态,不处理是否被bar */
            if ( NAS_MM_IsIdleState() == VOS_FALSE )
            {
                return ucEventID;
            }

            Mm_TimerStop(MM_TIMER_T3210);
            Mm_TimerStop(MM_TIMER_T3211);

            if ((VOS_TRUE == NAS_MML_GetSimCsRegStatus())
             && (MM_IDLE_NO_IMSI != g_MmGlobalInfo.ucState))
            {
                /*网络模式II，用户发起AT^CGCATT=0,2:
                处于WAIT_FOR_RR_CONNECTION_IMSI_DETACH状态，需要回复DETACH CNF
                  网络模式I，用户发起AT^CGCATT=0,2:
                处于IMSI_DETACH_PENDING状态，需要回复DETACH CNF
                */
                if ( ( WAIT_FOR_RR_CONNECTION_IMSI_DETACH == g_MmGlobalInfo.ucState )
                   ||( IMSI_DETACH_PENDING == g_MmGlobalInfo.ucState ) )
                {
                    /* 判断等待标志是否存在，如果存在则发送MMCMM_DETACH CNF */
                    if (MM_WAIT_CS_DETACH== (g_MmGlobalInfo.stDetachInfo.enDetachType
                                 & MM_WAIT_CS_DETACH))
                    {
                        NAS_MM_SndMmcDetachCnf(MM_LIMITED_SERVICE);
                        /* 清除等待标志 */
                        g_MmGlobalInfo.stDetachInfo.enDetachType = MM_WAIT_NULL_DETACH;
                    }
                }


                NAS_MM_SndMmcCsRegResultInd(MM_MMC_LU_PROC_FALSE,
                                            MM_MMC_LU_RESULT_FAILURE,
                                            NAS_MML_REG_FAIL_CAUSE_ACCESS_BARRED);
                NAS_MM_SndGmmLuResultInd(MMGMM_LU_FAILURE,
                                         NAS_MML_REG_FAIL_CAUSE_ACCESS_BARRED);

                /* 更新SYSINFO全局变量 */
                Mm_RcvMmcSysInfoInd(pRcvMsg);

                Mm_ComSetMmState(MM_IDLE_LOCATION_UPDATE_NEEDED);

                NAS_MM_RcvSysInfoAccessBar_ProcBufferedCsService();

                ucEventID = MM_EVENT_ID_INVALID;
            }
        }
        else
        {
            NAS_MM_SysInfoPreCheckClearAttemptCounter(pRcvMsg);
        }


    }

    return ucEventID;
}
VOS_VOID NAS_MM_ResetHplmnRejCauseChangedCounter (VOS_VOID)
{
    NAS_MML_HPLMN_REJ_CAUSE_CHANGED_COUNTER_STRU           *pstChangeCauseCounterInfo = VOS_NULL_PTR;

    pstChangeCauseCounterInfo = NAS_MML_GetHplmnRejCauseChangedCounter();

    pstChangeCauseCounterInfo->ucMmHplmnRejCauseChangedCounter = 0;

    return;
    
}


VOS_VOID NAS_MM_HandleHplmnRejCauseChange (
    VOS_UINT8                          *pucRcvMsg,                          
    VOS_UINT8                           ucConfigCauseNvim
)
{
    NAS_MML_HPLMN_REJ_CAUSE_CHANGED_COUNTER_STRU           *pstChangeCauseCounterInfo = VOS_NULL_PTR;

    pstChangeCauseCounterInfo = NAS_MML_GetHplmnRejCauseChangedCounter();

    /* 如果NV中配置的拒绝原因值为#17，则判断修改的次数是否已经达到上限，如果达到上限则不再修改为#17 */
    if (NAS_MML_REG_FAIL_CAUSE_NETWORK_FAILURE == ucConfigCauseNvim)
    {
        if (NAS_MML_MAX_HPLMN_REJ_CAUSE_CHANGE_COUNTER > pstChangeCauseCounterInfo->ucMmHplmnRejCauseChangedCounter)
        {
            pucRcvMsg[2] = ucConfigCauseNvim;
            pstChangeCauseCounterInfo->ucMmHplmnRejCauseChangedCounter++;
        }
    }
    else
    {
        pucRcvMsg[2] = ucConfigCauseNvim;    
    }

    return;

}
VOS_VOID NAS_MM_ChangeRegRejCauseAvoidInvalidSim (
    VOS_UINT8                          *pucRcvMsg,                          
    VOS_UINT16                          usMsgSize                           
)
{
    NAS_MML_PLMN_ID_STRU               *pstCurrCampPlmnId = VOS_NULL_PTR;
    VOS_UINT8                           ucRejCause;

    NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_ENUM_UINT8 enChangeRegRejType;

    enChangeRegRejType = NAS_MML_GetChangeRegRejCauFlg();
    ucRejCause = 0;
    
    /* 增加测试卡保护，测试卡时不修改，直接返回 */
    if (VOS_TRUE == NAS_USIMMAPI_IsTestCard())
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "NAS_MM_ChangeRegRejCauseAvoidInvalidSim: The sim is Test card");
        return;
    }

    if ((NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_CS_PS   != enChangeRegRejType)
     && (NAS_MML_CHANGE_REG_REJ_CAUSE_TYPE_CS_ONLY != enChangeRegRejType))
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "NAS_MM_ChangeRegRejCauseAvoidInvalidSim:WARNING: The Nvim is not actived!");
        return;
    }

    if ( usMsgSize < 3 )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "NAS_MM_ChangeRegRejCauseAvoidInvalidSim:WARNING: RCV MM LU REJECT MSG WITH ERROR SIZE!");
        return;
    }

    /* 取得当前驻留的PLMN ID */
    pstCurrCampPlmnId = NAS_MML_GetCurrCampPlmnId();
    
    ucRejCause = pucRcvMsg[2];

    if ((NAS_MML_REG_FAIL_CAUSE_IMSI_UNKNOWN_IN_HLR == ucRejCause)
     || (NAS_MML_REG_FAIL_CAUSE_ILLEGAL_MS == ucRejCause)
     || (NAS_MML_REG_FAIL_CAUSE_ILLEGAL_ME == ucRejCause))
    {
        /* 如果是HPLMN/EHPLMN则修改为#17，否则修改为#13 */
        if (VOS_TRUE == NAS_MML_ComparePlmnIdWithHplmn(pstCurrCampPlmnId))
        {
            NAS_MM_HandleHplmnRejCauseChange(pucRcvMsg, NAS_MML_GetPreferredRegRejCause_HPLMN_EHPLMN());
        }
        else
        {
            pucRcvMsg[2] = NAS_MML_GetPreferredRegRejCause_NOT_HPLMN_EHPLMN();
        }
    }
    
}
VOS_UINT8 Mm_ComMsgChkProc(
                        RRMM_DATA_IND_FOR_PCLINT_STRU       *pMsg               /* 当前处理的消息                           */
                        )
{
    VOS_UINT8                          *pucRcvMsg = pMsg->RcvNasMsg.aucNasMsg;                             /* 获得消息指针                             */
    VOS_UINT16                          usMsgSize = (VOS_UINT16)pMsg->RcvNasMsg.ulNasMsgSize;                  /* 获得消息的Size                           */
    VOS_UINT8                           ucEventId;                                                         /* 事件ID                                   */
    MM_MSG_MM_STATUS_STRU               Msg;

    if ( usMsgSize < 2 )
    {
        ucEventId = MM_EVENT_ID_INVALID;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgChkProc:WARNING: RCV MSG TOO SHORT!");
    }

    /* 完整性保护检查 */
    if (MM_FALSE == MM_ComCheckIntegrityProtection(pucRcvMsg, usMsgSize))
    {
        ucEventId = MM_EVENT_ID_INVALID;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgChkProc:WARNING:  Integrity Check Fail!");
    }
    else
    {
        /*收到NAS层消息，置全局变量为TRUE*/
        g_MmGlobalInfo.stMmLinkCtrl.ucSmcOrFirstNasMsgRcvdFlg = VOS_TRUE;

        switch ( pucRcvMsg[0] & 0x0f )
        {                                                                           /* 判断PD                                   */
        case MM_IE_PD_CALL_CONTROL:                                                 /* CC的消息                                 */
            ucEventId = Mm_ComMsgCcMsgInteChk(pMsg);
            break;
        case MM_IE_PD_SMS_MSG:                                                      /* SMS的消息                                */
            ucEventId = MM_EVENT_ID_SMS_MESSAGE;
            break;
        case MM_IE_PD_NON_CALL_RLT_SS_MSG:                                          /* SS的消息                                 */
        case MM_IE_PD_LOCATION_SRVC:                                                /* LCS的消息                                */
            ucEventId = MM_EVENT_ID_SS_MESSAGE;
            break;
        case MM_IE_PD_TEST:                                                         /* TC的消息                                 */
            ucEventId = MM_EVENT_ID_TC_MESSAGE;
            break;
        case MM_IE_PD_MM_MSG:                                                       /* MM的消息                                 */
            if ( 0 != ( pucRcvMsg[0] & 0xf0 ) )
            {
                ucEventId = MM_EVENT_ID_INVALID;
                PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgChkProc:WARNING: RCV MM MSG SKIP INDICATOR ERROR!");
                break;
            }

            MM_ReportN2MOtaMsg(pucRcvMsg[1] & 0x3f, (NAS_MSG_STRU *)(&(pMsg->RcvNasMsg)));


            switch ( pucRcvMsg[1] & 0x3f )
            {                                                                       /* 判断MM消息的类型                         */
            case MM_IE_MSG_TYPE_AUTH_REQ:                                           /* AUTHENTICATION REQUEST 消息              */
                ucEventId = Mm_ComMsgAuthReqRcv ( pucRcvMsg, usMsgSize );           /* 存储消息并获得事件ID                     */
                break;
            case MM_IE_MSG_TYPE_CM_SERV_REJ:                                        /* CM SERVICE REJECT 消息                   */
                ucEventId = Mm_ComMsgCmSvcRjctRcv ( pucRcvMsg, usMsgSize );         /* 存储消息并获得事件ID                     */
                g_T3211Flag = 0;
                break;
            case MM_IE_MSG_TYPE_CM_SERV_ACP:                                        /* CM SERVICE ACCEPT 消息                   */
                ucEventId = Mm_ComMsgCmSvcAcceptRcv(pMsg);                          /* 获得事件ID                               */
                g_T3211Flag = 0;
                break;
            case MM_IE_MSG_TYPE_AUTH_REJ:                                           /* AUTHENTICATION REJECT 消息               */
                ucEventId = Mm_ComMsgAuthRjctRcv();                                 /* 获得事件ID                               */
                break;
            case MM_IE_MSG_TYPE_ABORT:                                              /* ABORT 消息                               */
                ucEventId = Mm_ComMsgAbortRcv ( pucRcvMsg, usMsgSize );             /* 存储消息并获得事件ID                     */
                break;
            case MM_IE_MSG_TYPE_ID_REQ:                                             /* IDENTITY REQUEST 消息                    */
                ucEventId = Mm_ComMsgIdReqRcv ( pucRcvMsg, usMsgSize );             /* 存储消息并获得事件ID                     */
                break;
            case MM_IE_MSG_TYPE_MM_INFO:                                            /* MM INFORMATION 消息                      */
                ucEventId = Mm_ComMsgMmInfoRcv ( pucRcvMsg, usMsgSize );        /* 存储消息并获得事件ID */
                break;
            case MM_IE_MSG_TYPE_MM_STA:                                             /* MM STATUS 消息                           */
                ucEventId = Mm_ComMsgMmStatusRcv ( pucRcvMsg, usMsgSize );      /* 存储消息并获得事件ID                     */
                break;
            case MM_IE_MSG_TYPE_TMSI_REALLOC_CMD:                                   /* TMSI REALLOCATION COMMAND 消息           */
                ucEventId =
                            Mm_ComMsgTmsiReallocCmdRcv ( pucRcvMsg, usMsgSize );    /* 存储消息并获得事件ID   */
                break;
            case MM_IE_MSG_TYPE_LU_ACCEPT:
                ucEventId = Mm_ComMsgLuAccptRcv( pucRcvMsg, usMsgSize  );
                break;
            case MM_IE_MSG_TYPE_LU_REJECT:
                NAS_MM_ChangeRegRejCauseAvoidInvalidSim( pucRcvMsg, usMsgSize );
                ucEventId = Mm_ComMsgLuRejctRcv( pucRcvMsg, usMsgSize  );
                break;
            case MM_IE_MSG_TYPE_CM_SERV_PROMPT:
                ucEventId = Mm_ComMsgSvcPromptRcv( pucRcvMsg, usMsgSize );
                break;
            default:
                ucEventId = MM_EVENT_ID_INVALID;                                    /* 无效的事件ID                             */

                Msg.MmIeRjctCause.ucRejCause =
                                NAS_MML_REG_FAIL_CAUSE_MSG_NONEXIST_NOTIMPLEMENTE;
                Mm_ComMsgMmStatusSnd(&Msg);

                break;
            }
            break;
        default:
            ucEventId = MM_EVENT_ID_INVALID;                                        /* 无效的事件ID                             */
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgChkProc:WARNING: RCV NAS MSG PD UNKNOW!");
            break;
        }
    }

    return ucEventId;                                                           /* 返回事件ID                               */
}

/*******************************************************************************
  Module:   Mm_ComMsgAuthRspSnd
  Function: 填充并发送 AUTHENTICATION RESPONSE 消息
  Input:    MM_MSG_AUTHENTICATION_RSP_STRU *pMsg,要发送的消息的指针
  Output:
  NOTE:
  Return:   VOS_VOID
  History:
      1.  张志勇   2003.12.10   新版作成
*******************************************************************************/
VOS_VOID Mm_ComMsgAuthRspSnd(
                          MM_MSG_AUTHENTICATION_RSP_STRU    *pMsg               /* 要发送的消息的指针                       */
                         )
{
    VOS_UINT8               i;                                                  /* 循环变量                                 */
    VOS_UINT16              usMsgSize = 20;                                     /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */

    if ( MM_IE_AUTN_EXT_PRESENT == pMsg->ucAutnRspParaExtFlg )
    {                                                                           /* AUTN Extention 存在                      */
        usMsgSize = (VOS_UINT16)(8 + pMsg->MmIeAuthRspParaExt.ucLength);
    }
    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgAuthRspSnd:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_AUTH_RES;                                 /* 设置Message type                         */
        for ( i = MM_CONST_NUM_0; i < MM_CONST_NUM_4; i++ )
        {
            pucSndMsg[2+i] = pMsg->MmIeAuthRspPara.aucSgnfcntRES[i];            /* 设置AUTN                                 */
        }
        if ( MM_IE_AUTN_EXT_PRESENT == pMsg->ucAutnRspParaExtFlg )
        {                                                                       /* AUTN Extention 存在                      */
            pucSndMsg[6] = 0x21;                                                /* 设置AUTN Extention的IEI                  */
            pucSndMsg[7] = pMsg->MmIeAuthRspParaExt.ucLength;                   /* 设置Length of AUTN Extention             */
            for(i = MM_CONST_NUM_0; i < pMsg->MmIeAuthRspParaExt.ucLength; i++)
            {
                pucSndMsg[8+i] = pMsg->MmIeAuthRspParaExt.aucRES[i];            /* 设置AUTN Extention的内容                 */
            }
            usMsgSize = (VOS_UINT16)(8 + pMsg->MmIeAuthRspParaExt.ucLength);    /* 调整消息实际长度                         */
        }
        else
        {                                                                       /* AUTN Extention 不存在                    */
            usMsgSize = 6;                                                      /* 调整消息实际长度                         */
        }

        #ifdef GSM_GCF_RS_SIM_STUB
        pucSndMsg[2] = 0x11;
        pucSndMsg[3] = 0x10;
        pucSndMsg[4] = 0x13;
        pucSndMsg[5] = 0x12;
        usMsgSize    = 6;
        #endif
        Mm_SndRrDataReq((VOS_UINT32)usMsgSize, pucSndMsg, RRC_NAS_MSG_PRIORTY_HIGH);

        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}

/*******************************************************************************
  Module:   Mm_ComMsgAuthFailSnd
  Function: 填充并发送 AUTHENTICATION FAILURE 消息
  Input:    MM_MSG_AUTHENTICATION_FAIL_STRU *pMsg,要发送的消息的指针
  Output:
  NOTE:
  Return:   VOS_VOID
  History:
      1.  张志勇   2003.12.10   新版作成
*******************************************************************************/
VOS_VOID Mm_ComMsgAuthFailSnd(
                          MM_MSG_AUTHENTICATION_FAIL_STRU    *pMsg              /* 要发送的消息的指针                       */
                            )
{
    VOS_UINT8               i;                                                      /* 循环变量                                 */
    VOS_UINT16              usMsgSize = 19;                                         /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                                       /* 定义消息指针                             */

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgAuthFailSnd:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_AUTH_FAIL;                                /* 设置Message type                         */
        pucSndMsg[2] = pMsg->MmIeRejCause.ucRejCause;                           /* 设置Reject Cause                         */

        if ( NAS_MML_REG_FAIL_CAUSE_SYNCH_FAILURE == pMsg->MmIeRejCause.ucRejCause )
        {                                                                       /* 拒绝原因为 Synch failure                 */
            pucSndMsg[3] = 0x22;                                                /* 设置Authentication Failure parameter的IE
                                                                                 * I                                        */
            pucSndMsg[4] = pMsg->MmIeAuthFailPara.ucLength;                     /* 设置Length of Authentication Failurepara
                                                                                 * meter contents                           */
            for (i = MM_CONST_NUM_0; i < pMsg->MmIeAuthFailPara.ucLength; i++)
            {
                pucSndMsg[5+i] = pMsg->MmIeAuthFailPara.aucAuthFailPara[i];     /* 设置Authentication Failure parameter     */
            }
        }
        else
        {                                                                       /* 拒绝原因为 MAC failure                   */
            usMsgSize = 3;                                                      /* 调整消息实际长度                         */
        }

        Mm_SndRrDataReq((VOS_UINT32)usMsgSize, pucSndMsg, RRC_NAS_MSG_PRIORTY_HIGH);

        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}
VOS_UINT8 Mm_ComMsgSetMobileId(
                            VOS_UINT8                  *pucSndMsg,              /* 消息的首地址                             */
                            VOS_UINT16                 *pusMsgSize,             /* 消息目前的长度                           */
                            MM_MOBILE_ID_STRU          *pMobileId               /* 源数据指针                               */
                            )
{
    VOS_UINT8                           i;                                                  /* 循环变量                                 */
    VOS_UINT16                          usIndex = *pusMsgSize;                              /* 位置索引                                 */
    VOS_UINT8                          *pTmpImsi;

    /* TMSI有效，且当前驻留网络与分配该TMSI的网络为相同网络或等效PLMN。 */
    if (MM_MS_ID_TMSI_PRESENT == (pMobileId->ucMsIdFlg & MM_MS_ID_TMSI_PRESENT))
    {

        pucSndMsg[usIndex] = 0x05;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = 0xf0 | MM_IE_MI_ID_TYPE_TMSI;                    /* 设置Type of identity 和奇偶标志位        */
        for ( i = MM_CONST_NUM_0; i < MM_CONST_NUM_4; i++ )
        {
            pucSndMsg[usIndex+2+i]
                = (VOS_UINT8)(pMobileId->aucTmsi[i]);                           /* 设置TMSI的内容                           */
        }
        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }
    else if ( MM_MS_ID_IMSI_PRESENT ==
        (pMobileId->ucMsIdFlg & MM_MS_ID_IMSI_PRESENT) )
    {

        pTmpImsi = NAS_MML_GetSimImsi();
        PS_MEM_CPY(&pucSndMsg[usIndex],
                   pTmpImsi,
                   pTmpImsi[0]+1);

        *pusMsgSize = (VOS_UINT16)(usIndex + pTmpImsi[0] + 1);           /* 调整消息实际长度                         */
    }
    else if ( MM_MS_ID_IMEI_PRESENT ==
        (pMobileId->ucMsIdFlg & MM_MS_ID_IMEI_PRESENT) )
    {
        pucSndMsg[usIndex] = 0x08;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = (VOS_UINT8)(pMobileId->aucImei[0] & 0xf0);       /* 设置pucSndMsg[usIndex+1]的高四位为digit1 */
        pucSndMsg[usIndex+1] |= MM_IE_MI_ID_TYPE_IMEI | 0x08;                   /* 设置pucSndMsg[usIndex+1]低4位为奇偶标志
                                                                                 * 位和Typeof identity                      */
        for ( i = MM_CONST_NUM_1; i < pucSndMsg[usIndex]; i++ )
        {
            pucSndMsg[usIndex+1+i] = pMobileId->aucImei[i];                     /* 设置IMEI剩余的digits                     */
        }
        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }
    else if ( MM_MS_ID_IMEISV_PRESENT ==
        (pMobileId->ucMsIdFlg & MM_MS_ID_IMEISV_PRESENT) )
    {
        pucSndMsg[usIndex] = 0x09;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = (VOS_UINT8)(pMobileId->aucImeisv[0] & 0xf0);     /* 设置pucSndMsg[usIndex+1]高四位为digit 1  */
        pucSndMsg[usIndex+1] |= MM_IE_MI_ID_TYPE_IMEISV;                        /* 设置pucSndMsg[usIndex+1]低4位为奇偶标志
                                                                                   位和Typeof identity                      */
        for ( i = MM_CONST_NUM_1; i < pucSndMsg[usIndex]; i++ )
        {
            pucSndMsg[usIndex+1+i] = pMobileId->aucImeisv[i];                   /* 设置IMEISV剩余的digits                   */
        }
        pucSndMsg[usIndex+pucSndMsg[usIndex]] |= 0xf0;                          /* 设置消息最后一个字节的高四位为"1111"     */
        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }
    else
    {
        pucSndMsg[usIndex] = 0x01;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = 0xf0;                                            /* 设置pucSndMsg[usIndex+1]低4位为奇偶标志
                                                                                   位和Typeof identity, 高四位为"1111"      */
        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }

    return MM_TRUE;                                                             /* 返回                                     */
}



VOS_UINT8 NAS_MM_SetPagingRspMobileId(
    VOS_UINT8                          *pucSndMsg,
    VOS_UINT16                         *pusMsgSize,
    MM_MOBILE_ID_STRU                  *pMobileId
)
{
    VOS_UINT8                           i;                                                  /* 循环变量                                 */
    VOS_UINT16                          usIndex = *pusMsgSize;                             /* 位置索引                                 */
    VOS_UINT8                          *pTmpImsi;

    /* TMSI有效，且当前驻留网络与分配该TMSI的网络并不要求为相同网络或等效PLMN。 */
    if (((MM_TMSI_PAGING == g_MmGlobalInfo.ucPagingRecordTypeId)
      || (MM_STMSI_PAGING == g_MmGlobalInfo.ucPagingRecordTypeId))
     && (MM_MS_ID_TMSI_PRESENT == (pMobileId->ucMsIdFlg & MM_MS_ID_TMSI_PRESENT)))
    {
        pucSndMsg[usIndex] = 0x05;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = 0xf0 | MM_IE_MI_ID_TYPE_TMSI;                    /* 设置Type of identity 和奇偶标志位        */
        for ( i = MM_CONST_NUM_0; i < MM_CONST_NUM_4; i++ )
        {
            pucSndMsg[usIndex+2+i]
                = (VOS_UINT8)(pMobileId->aucTmsi[i]);                           /* 设置TMSI的内容                           */
        }
        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }
    else if ( MM_MS_ID_IMSI_PRESENT ==
        (pMobileId->ucMsIdFlg & MM_MS_ID_IMSI_PRESENT) )
    {

        pTmpImsi = NAS_MML_GetSimImsi();
        PS_MEM_CPY(&pucSndMsg[usIndex],
                   pTmpImsi,
                   pTmpImsi[0]+1);

        *pusMsgSize = (VOS_UINT16)(usIndex + pTmpImsi[0] + 1);           /* 调整消息实际长度                         */
    }
    else if ( MM_MS_ID_IMEI_PRESENT ==
        (pMobileId->ucMsIdFlg & MM_MS_ID_IMEI_PRESENT) )
    {
        pucSndMsg[usIndex] = 0x08;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = (VOS_UINT8)(pMobileId->aucImei[0] & 0xf0);       /* 设置pucSndMsg[usIndex+1]的高四位为digit1 */
        pucSndMsg[usIndex+1] |= MM_IE_MI_ID_TYPE_IMEI | 0x08;                   /* 设置pucSndMsg[usIndex+1]低4位为奇偶标志
                                                                                 * 位和Typeof identity                      */
        for ( i = MM_CONST_NUM_1; i < pucSndMsg[usIndex]; i++ )
        {
            pucSndMsg[usIndex+1+i] = pMobileId->aucImei[i];                     /* 设置IMEI剩余的digits                     */
        }
        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }
    else if ( MM_MS_ID_IMEISV_PRESENT ==
        (pMobileId->ucMsIdFlg & MM_MS_ID_IMEISV_PRESENT) )
    {
        pucSndMsg[usIndex] = 0x09;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = (VOS_UINT8)(pMobileId->aucImeisv[0] & 0xf0);     /* 设置pucSndMsg[usIndex+1]高四位为digit 1  */
        pucSndMsg[usIndex+1] |= MM_IE_MI_ID_TYPE_IMEISV;                        /* 设置pucSndMsg[usIndex+1]低4位为奇偶标志
                                                                                   位和Typeof identity                      */
        for ( i = MM_CONST_NUM_1; i < pucSndMsg[usIndex]; i++ )
        {
            pucSndMsg[usIndex+1+i] = pMobileId->aucImeisv[i];                   /* 设置IMEISV剩余的digits                   */
        }
        pucSndMsg[usIndex+pucSndMsg[usIndex]] |= 0xf0;                          /* 设置消息最后一个字节的高四位为"1111"     */
        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }
    else
    {
        pucSndMsg[usIndex] = 0x01;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = 0xf0;                                            /* 设置pucSndMsg[usIndex+1]低4位为奇偶标志
                                                                                   位和Typeof identity, 高四位为"1111"      */
        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }

    return MM_TRUE;                                                             /* 返回                                     */
}



VOS_UINT8 Mm_ComMsgSetMobileIdForEmc(
                            VOS_UINT8                  *pucSndMsg,              /* 消息的首地址                             */
                            VOS_UINT16                 *pusMsgSize,             /* 消息目前的长度                           */
                            MM_MOBILE_ID_STRU          *pMobileId               /* 源数据指针                               */
                            )
{
    VOS_UINT8               i;                                                  /* 循环变量                                 */
    VOS_UINT16              usIndex = *pusMsgSize;                              /* 位置索引                                 */
    NAS_MML_LOCATION_UPDATE_STATUS_ENUM_UINT8               enCsUpdateStatus;
    VOS_UINT8              *pucIMSI;

    enCsUpdateStatus = NAS_MML_GetCsUpdateStatus();

    if ( (MM_FALSE == Mm_ComLaiJudge())
      && (NAS_MML_LOCATION_UPDATE_STATUS_UPDATED == enCsUpdateStatus)
      && (MM_MS_ID_TMSI_PRESENT == (pMobileId->ucMsIdFlg & MM_MS_ID_TMSI_PRESENT)))
    {
        pucSndMsg[usIndex] = 0x05;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = 0xf0 | MM_IE_MI_ID_TYPE_TMSI;                    /* 设置Type of identity 和奇偶标志位        */

        for ( i = MM_CONST_NUM_0; i < MM_CONST_NUM_4; i++ )
        {
            pucSndMsg[usIndex+2+i]
                = (VOS_UINT8)(pMobileId->aucTmsi[i]);                           /* 设置TMSI的内容                           */
        }

        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }

    else if (MM_MS_ID_IMSI_PRESENT ==
        (pMobileId->ucMsIdFlg & MM_MS_ID_IMSI_PRESENT))
    {

        pucIMSI = NAS_MML_GetSimImsi();

        pucSndMsg[usIndex] = pucIMSI[0];


        PS_MEM_CPY(&pucSndMsg[usIndex+1],
               pucIMSI + 1,
               pucIMSI[0]);

        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }

    else if (MM_MS_ID_IMEI_PRESENT ==
        (pMobileId->ucMsIdFlg & MM_MS_ID_IMEI_PRESENT))
    {
        pucSndMsg[usIndex] = 0x08;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = (VOS_UINT8)(pMobileId->aucImei[0] & 0xf0);       /* 设置pucSndMsg[usIndex+1]的高四位为digit1 */
        pucSndMsg[usIndex+1] |= MM_IE_MI_ID_TYPE_IMEI | 0x08;                   /* 设置pucSndMsg[usIndex+1]低4位为奇偶标志
                                                                                 * 位和Typeof identity                      */
        for ( i = MM_CONST_NUM_1; i < pucSndMsg[usIndex]; i++ )
        {
            pucSndMsg[usIndex+1+i] = pMobileId->aucImei[i];                     /* 设置IMEI剩余的digits                     */
        }

        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }

    else
    {
        pucSndMsg[usIndex] = 0x01;                                              /* 设置Length of mobile identity contents   */
        pucSndMsg[usIndex+1] = MM_IE_MI_ID_TYPE_NO_ID | 0x08;                   /* 设置pucSndMsg[usIndex+1]低4位为奇偶标志
                                                                                   位和Typeof identity, 高四位为"1111"      */
        *pusMsgSize = (VOS_UINT16)(usIndex + pucSndMsg[usIndex] + 1);           /* 调整消息实际长度                         */
    }

    return MM_TRUE;
}
VOS_VOID Mm_ComMsgCmSvcReqSnd(
                          MM_MSG_CM_SVC_REQ_STRU    *pMsg                       /* 要发送的消息的指针                       */
                            )
{
    VOS_UINT16              usMsgSize = 18;                                     /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgCmSvcReqSnd:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                 /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_CM_SERV_REQ;                              /* 设置Message type                         */

        pucSndMsg[2] = (VOS_UINT8)(pMsg->MmIeCmSvcType.ucCMSvcType & 0x0f);         /* 设置CM service type                      */
        pucSndMsg[2] = (VOS_UINT8)(pucSndMsg[2] | (pMsg->MmIeCKSN.ucCksn << 4));    /* 设置CKSN                                 */

        MM_Fill_IE_ClassMark2(&pucSndMsg[3]);

        usMsgSize = 7;                                                          /* 调整消息实际长度                         */
        if ( MM_TRUE == Mm_ComMsgSetMobileId (pucSndMsg, &usMsgSize,
                                                &pMsg->MmIeMobileId.MobileID))
        {
            if ( MM_TRUE == pMsg->ucPrioLeveFlg )
            {                                                                   /* 如果存在Priority                         */
                pucSndMsg[usMsgSize] = (VOS_UINT8)(0x80
                    | (pMsg->MmIePrio.ucPrioLeve & MM_IE_PRIO_LEVE_MASK));      /* 设置Priority的IEI和具体内容              */
                usMsgSize++;                                                    /* 调整消息实际长度                         */
            }

            Mm_SndRrDataReq((VOS_UINT32)usMsgSize, pucSndMsg, RRC_NAS_MSG_PRIORTY_HIGH);

        }
        else
        {
            /*WUEPS_ASSERT(0);
 */
        }
        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                    /* 释放为发送消息申请的内存空间             */
        Mm_TimerStop(MM_TIMER_T3211);
    }
    return;                                                                     /* 返回                                     */
}


VOS_UINT8* Mm_ComMsgCmSvcReqOfRrEstReq(
                                    MM_MSG_CM_SVC_REQ_STRU    *pMsg,            /* 要发送的消息的指针                       */
                                    VOS_UINT16                *pusMsgSize       /* 消息的长度指针                           */
                                    )
{
    VOS_UINT16              usMsgSize = 18;                                     /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */

    *pusMsgSize = 0;                                                            /* 消息长度初始化为0                        */

    pucSndMsg = (VOS_UINT8 *)MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR != pucSndMsg )
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_CM_SERV_REQ;                              /* 设置Message type                         */

        pucSndMsg[2] = (VOS_UINT8)(pMsg->MmIeCmSvcType.ucCMSvcType & 0x0f);     /* 设置CM service type                      */
        pucSndMsg[2] = (VOS_UINT8)(pucSndMsg[2] | (pMsg->MmIeCKSN.ucCksn << 4));/* 设置CKSN                                 */

        MM_Fill_IE_ClassMark2(&pucSndMsg[3]);

        usMsgSize = 7;                                                          /* 调整消息实际长度                         */

        if (MM_IE_CM_SVC_TYPE_EMG_CALL_EST == pMsg->MmIeCmSvcType.ucCMSvcType)
        {
            if (MM_TRUE == Mm_ComMsgSetMobileIdForEmc(pucSndMsg, &usMsgSize,
                                                    &pMsg->MmIeMobileId.MobileID))
            {
                if (MM_TRUE == pMsg->ucPrioLeveFlg)
                {                                                               /* 如果存在Priority                         */
                    pucSndMsg[usMsgSize] = (VOS_UINT8)(0x80
                        | (pMsg->MmIePrio.ucPrioLeve & MM_IE_PRIO_LEVE_MASK));  /* 设置Priority的IEI和具体内容              */
                    usMsgSize++;                                                /* 调整消息实际长度                         */
                }
                *pusMsgSize = usMsgSize;                                        /* 设置消息的长度,作输出参数使用            */
            }
            else
            {
                MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                  /* 释放为发送消息申请的内存空间             */
                pucSndMsg = VOS_NULL_PTR;                                       /* 指针赋空                                 */
                *pusMsgSize = 0;                                                /* 消息长度置为0                            */
            }
        }
        else
        {
            if (MM_TRUE == Mm_ComMsgSetMobileId(pucSndMsg, &usMsgSize,
                                                &pMsg->MmIeMobileId.MobileID))
            {
                if (MM_TRUE == pMsg->ucPrioLeveFlg)
                {                                                               /* 如果存在Priority                         */
                    pucSndMsg[usMsgSize] = (VOS_UINT8)(0x80
                        | (pMsg->MmIePrio.ucPrioLeve & MM_IE_PRIO_LEVE_MASK));  /* 设置Priority的IEI和具体内容              */
                    usMsgSize++;                                                /* 调整消息实际长度                         */
                }
                *pusMsgSize = usMsgSize;                                        /* 设置消息的长度,作输出参数使用            */
            }
            else
            {
                MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                  /* 释放为发送消息申请的内存空间             */
                pucSndMsg = VOS_NULL_PTR;                                       /* 指针赋空                                 */
                *pusMsgSize = 0;                                                /* 消息长度置为0                            */
            }
        }

        if (MMCC_MO_NORMAL_CALL == g_MmCcEstReq.ulCallType)
        {
            Mm_TimerStop(MM_TIMER_T3211);
        }
    }
    else
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgCmSvcReqOfRrEstReq:ERROR: MALLOC ERROR!");
    }
    return pucSndMsg;                                                           /* 返回                                     */
}
VOS_VOID Mm_ComMsgCmReEstReqSnd(
                            MM_MSG_RE_EST_REQ_STRU    *pMsg                     /* 要发送的消息的指针                       */
                            )
{
    VOS_UINT16              usMsgSize = 23;                                     /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgCmReEstReqSnd:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_REEST_REQ;                                /* 设置Message type                         */

        pucSndMsg[2]
            = (VOS_UINT8)(pMsg->MmIeCKSN.ucCksn & MM_IE_CKSN_KEY_SEQUENCE_MASK);/* 设置CKSN                                 */

        MM_Fill_IE_ClassMark2(&pucSndMsg[3]);

        usMsgSize = 7;                                                          /* 调整消息实际长度                         */
        if ( MM_TRUE == Mm_ComMsgSetMobileId (pucSndMsg, &usMsgSize,
                                                &pMsg->MmIeMobileId.MobileID))
        {
            if (MM_MS_ID_TMSI_PRESENT
                == ( MM_MS_ID_TMSI_PRESENT &
                pMsg->MmIeMobileId.MobileID.ucMsIdFlg ) )
            {                                                                   /* 如果Mobile Id为TMSI,接着要设置LAI        */
                pucSndMsg[usMsgSize] = 0x13;                                    /* 设置LAI的IEI                             */
                pucSndMsg[usMsgSize+1]
                    = (VOS_UINT8)((pMsg->MmIeLai.IeLai.PlmnId.ulMcc >> 4)
                                | pMsg->MmIeLai.IeLai.PlmnId.ulMcc );           /* 设置MCC digit 2,digit 1                  */
                pucSndMsg[usMsgSize+2]
                    = (VOS_UINT8)((pMsg->MmIeLai.IeLai.PlmnId.ulMcc >> 16)
                                | (pMsg->MmIeLai.IeLai.PlmnId.ulMnc >> 12));    /* 设置MNC digit 3, MCC digit 3             */
                pucSndMsg[usMsgSize+3]
                    = (VOS_UINT8)((pMsg->MmIeLai.IeLai.PlmnId.ulMnc >> 4)
                                | pMsg->MmIeLai.IeLai.PlmnId.ulMnc );           /* 设置MNC digit 2,digit 1                  */

                pucSndMsg[usMsgSize+4] = (VOS_UINT8)(pMsg->MmIeLai.IeLai.ulLac>>8);      /* 设置LAC高8位                             */
                pucSndMsg[usMsgSize+5] = (VOS_UINT8)pMsg->MmIeLai.IeLai.ulLac; /* 设置LAC低8位                             */

                usMsgSize += 6;                                                 /* 调整消息实际长度                         */

            }
            if (MMCC_EMERGENCY_CALL == g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].RcvXXEstReq.ulCallType)
            {
                Mm_SndRrEstReq( RRC_EST_CAUSE_EMERGENCY_CALL_RE_ESTABLISH, MM_FALSE,
                    usMsgSize, pucSndMsg );                                         /* 发送消息                                 */
            }
            else
            {
                Mm_SndRrEstReq( RRC_EST_CAUSE_CALL_RE_ESTABLISH, MM_FALSE,
                    usMsgSize, pucSndMsg );                                         /* 发送消息                                 */
            }
        }
        else
        {
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgCmReEstReqSnd:WARNING: MSG SET MOBILE ID ERROR!");
        }
        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}

/*******************************************************************************
  Module:   Mm_ComMsgIdRspSnd
  Function: 填充并发送 IDENTITY RESPONSE 消息
  Input:    MM_MSG_ID_RSP_STRU *pMsg,要发送的消息的指针
  Output:
  NOTE:
  Return:   VOS_VOID
  History:
      1.  张志勇   2003.12.11   新版作成
*******************************************************************************/
VOS_VOID Mm_ComMsgIdRspSnd(
                          MM_MSG_ID_RSP_STRU    *pMsg                           /* 要发送的消息的指针                       */
                            )
{
    VOS_UINT16              usMsgSize = 12;                                     /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgIdRspSnd:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_ID_RES;                                   /* 设置Message type                         */

        usMsgSize = 2;                                                          /* 调整消息实际长度                         */
        if ( MM_TRUE == Mm_ComMsgSetMobileId (pucSndMsg, &usMsgSize,
                                                &pMsg->MmIeMobileId.MobileID))
        {
            Mm_SndRrDataReq((VOS_UINT32)usMsgSize, pucSndMsg, RRC_NAS_MSG_PRIORTY_HIGH);
        }
        else
        {
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgIdRspSnd:WARNING: MSG SET MOBILE ID ERROR!");
        }
        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}

/*******************************************************************************
  Module:   Mm_ComMsgMmStatusSnd
  Function: 填充并发送 MM STATUS 消息
  Input:    MM_MSG_MM_STATUS_STRU *pMsg,要发送的消息的指针
  Output:
  NOTE:
  Return:   VOS_VOID
  History:
      1.  张志勇   2003.12.11   新版作成
*******************************************************************************/
VOS_VOID Mm_ComMsgMmStatusSnd(
                          MM_MSG_MM_STATUS_STRU    *pMsg                        /* 要发送的消息的指针                       */
                            )
{
    VOS_UINT16              usMsgSize = 3;                                      /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgMmStatusSnd:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_MM_STA;                                   /* 设置Message type                         */
        pucSndMsg[2] = pMsg->MmIeRjctCause.ucRejCause;                          /* 设置Reject Cause                         */

        Mm_SndRrDataReq((VOS_UINT32)usMsgSize, pucSndMsg, RRC_NAS_MSG_PRIORTY_HIGH);

        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}

/*******************************************************************************
  Module:   Mm_ComMsgTmsiRelocCpltSnd
  Function: 填充并发送 TMSI REALLOCATION COMPLETE 消息
  Input:    VOS_VOID
  Output:
  NOTE:
  Return:   VOS_VOID
  History:
      1.  张志勇   2003.12.11   新版作成
*******************************************************************************/
VOS_VOID Mm_ComMsgTmsiRelocCpltSnd( )
{
    VOS_UINT16              usMsgSize = 2;                                      /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgTmsiRelocCpltSnd:ERROR: MALLOC ERROR!");
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_TMSI_REALLOC_CPL;                         /* 设置Message type                         */

        Mm_SndRrDataReq((VOS_UINT32)usMsgSize, pucSndMsg, RRC_NAS_MSG_PRIORTY_HIGH);

        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}


VOS_VOID Mm_ComMsgPagingRspSnd(
                            MM_MSG_PAGING_RSP_STRU    *pMsg                     /* 要发送的消息的指针                       */
                            )
{
    VOS_UINT16              usMsgSize = 17;                                     /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */
    VOS_UINT8               ucEstCause  = 0;                                    /* RRC建立的原因                            */
    NAS_MML_CSFB_SERVICE_STATUS_ENUM_UINT8                  enCsfbService ;

    enCsfbService = NAS_MML_GetCsfbServiceStatus();

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgPagingRspSnd:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */

        pucSndMsg[0] = MM_IE_PD_RRM_MSG;                                        /* 设置PD, Skip Indicator                   */

        pucSndMsg[1] = MM_IE_MSG_TYPE_PAGING_RSP;                               /* 设置Message type                         */

        pucSndMsg[2]
            = (VOS_UINT8)(pMsg->MmIeCksn.ucCksn & MM_IE_CKSN_KEY_SEQUENCE_MASK);    /* 设置CKSN                                 */

        MM_Fill_IE_ClassMark2(&pucSndMsg[3]);
        usMsgSize = 7;                                                          /* 调整消息实际长度                         */
        switch ( g_MmGlobalInfo.ucPagingCause )
            {                                                                   /* RRMM_PAGING_IND与RRMM_EST_REQ中cause转换 */
            case RRC_PAGE_CAUSE_TERMINAT_CONVERSAT_CALL:
                if ( (VOS_TRUE == NAS_MML_GetRelCauseCsfbHighPrioFlg())
                    && (NAS_MML_CSFB_SERVICE_STATUS_MT_EXIST == enCsfbService) )
                {
                    ucEstCause = RRC_EST_CAUSE_TERMINAT_HIGH_PRIORITY_SIGNAL;
                }
                else
                {
                    ucEstCause = RRC_EST_CAUSE_TERMINAT_CONVERSAT_CALL;
                }
                break;
            case RRC_PAGE_CAUSE_TERMINAT_STREAM_CALL:
                ucEstCause = RRC_EST_CAUSE_TERMINAT_STREAM_CALL;
                break;
            case RRC_PAGE_CAUSE_TERMINAT_INTERACT_CALL:
                ucEstCause = RRC_EST_CAUSE_TERMINAT_INTERACT_CALL;
                break;
            case RRC_PAGE_CAUSE_TERMINAT_BACKGROUND_CALL:
                ucEstCause = RRC_EST_CAUSE_TERMINAT_BACKGROUND_CALL;
                break;
            case RRC_PAGE_CAUSE_TERMINAT_HIGH_PRIORITY_SIGNAL:
                ucEstCause = RRC_EST_CAUSE_TERMINAT_HIGH_PRIORITY_SIGNAL;
                break;
            case RRC_PAGE_CAUSE_TERMINAT_LOW_PRIORITY_SIGNAL:
                ucEstCause = RRC_EST_CAUSE_TERMINAT_LOW_PRIORITY_SIGNAL;
                break;
            case RRC_PAGE_CAUSE_TERMINAT_CAUSE_UNKNOWN:
                ucEstCause = RRC_EST_CAUSE_TERMINAT_CAUSE_UNKNOWN;
                break;
            default:
                ucEstCause = RRC_EST_CAUSE_TERMINAT_CAUSE_UNKNOWN;
                break;
            }

        if ( MM_TRUE == NAS_MM_SetPagingRspMobileId (pucSndMsg, &usMsgSize,
                                                &pMsg->MmIeMobileId.MobileID) )
        {
            Mm_SndRrEstReq( ucEstCause, MM_TRUE,
                usMsgSize, pucSndMsg );                                         /* 发送消息                                 */
        }
        else
        {
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgPagingRspSnd:WARNING: MSG SET MOBILE ID ERROR!");
        }
        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}

/*******************************************************************************
  Module:   Mm_ComMsgCmSvcAbortSnd
  Function: 填充并发送 CM SERVICE ABORT 消息
  Input:    VOS_VOID
  Output:
  NOTE:
  Return:   VOS_VOID
  History:
      1.  张志勇   2003.12.11   新版作成
*******************************************************************************/
VOS_VOID Mm_ComMsgCmSvcAbortSnd( )
{
    VOS_UINT16              usMsgSize = 2;                                      /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                          /* 定义消息指针                             */

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgCmSvcAbortSnd:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_CM_SERV_ABO;                              /* 设置Message type                         */

        Mm_SndRrDataReq((VOS_UINT32)usMsgSize, pucSndMsg, RRC_NAS_MSG_PRIORTY_HIGH);

        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}


VOS_VOID Mm_ComMsgImsiDetachIniSndOfEstReq(
                                       MM_MSG_IMSI_DETACH_IND_STRU    *pMsg     /* 要发送的消息的指针                       */
                                       )
{
    VOS_UINT16              usMsgSize = 13;                                     /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgImsiDetachIniSndOfEstReq:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_IMSI_DETACH_INDICATION;                   /* 设置Message type                         */

        /* 设置Mobile station classmark1             */
        MM_Fill_IE_ClassMark1(&pucSndMsg[2]);

        usMsgSize = 3;                                                          /* 调整消息实际长度                         */

        if ( MM_TRUE == Mm_ComMsgSetMobileId (pucSndMsg, &usMsgSize,
                                                &pMsg->MmIeMobileId.MobileID) )
        {
            Mm_SndRrEstReq( RRC_EST_CAUSE_DETACH, MM_FALSE,
                usMsgSize, pucSndMsg );                                         /* 发送消息                                 */
        }
        else
        {
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgImsiDetachIniSndOfEstReq:WARNING: MSG SET MOBILE ID ERROR!");
        }
        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}


VOS_VOID Mm_ComMsgImsiDetachIniSndOfDataReq(
                                       MM_MSG_IMSI_DETACH_IND_STRU    *pMsg     /* 要发送的消息的指针                       */
                                       )
{
    VOS_UINT16              usMsgSize = 13;                                     /* 消息长度,以字节为单位                    */
    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgImsiDetachIniSndOfDataReq:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[0] = MM_IE_PD_MM_MSG;                                         /* 设置PD, Skip Indicator                   */
        pucSndMsg[1] = MM_IE_MSG_TYPE_IMSI_DETACH_INDICATION;                   /* 设置Message type                         */

        /* 设置Mobile station classmark1             */
        MM_Fill_IE_ClassMark1(&pucSndMsg[2]);

        usMsgSize = 3;                                                          /* 调整消息实际长度                         */

        if ( MM_TRUE == Mm_ComMsgSetMobileId (pucSndMsg, &usMsgSize,
                                                &pMsg->MmIeMobileId.MobileID) )
        {
            Mm_SndRrDataReq((VOS_UINT32)usMsgSize, pucSndMsg,
                RRC_NAS_MSG_PRIORTY_HIGH );                                     /* 发送消息                                 */
        }
        else
        {
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgImsiDetachIniSndOfDataReq:WARNING: MSG SET MOBILE ID ERROR!");
        }
        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}


VOS_VOID Mm_ComMsgLuReqSndOfEstReq(
                               MM_MSG_LU_REQ_STRU    *pMsg                      /* 要发送的消息的指针                       */
                               )
{
    VOS_UINT16              usMsgSize = 24;                                     /* 消息长度,以字节为单位                    */

    VOS_UINT8              *pucSndMsg = VOS_NULL_PTR;                           /* 定义消息指针                             */
    VOS_UINT16              usIdx = 0;
    VOS_UINT8               i;

    pucSndMsg = (VOS_UINT8 *) MM_MEM_ALLOC(
                                      VOS_MEMPOOL_INDEX_MM,
                                      usMsgSize,
                                      WUEPS_MEM_NO_WAIT
                                       );                                       /* 申请发送的消息所用的最大内存空间         */
    if ( VOS_NULL_PTR == pucSndMsg )
    {

        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_ERROR, "Mm_ComMsgLuReqSndOfEstReq:ERROR: MALLOC ERROR!");
        return;
    }
    else
    {
        PS_MEM_SET( pucSndMsg, 0, usMsgSize );                                  /* 清零                                     */
        pucSndMsg[usIdx] =  MM_IE_PD_MM_MSG;                                    /* 设置PD                                   */
        pucSndMsg[usIdx++] &= 0x0f;                                             /* Skip Indicator                           */
        pucSndMsg[usIdx++] = MM_IE_MSG_TYPE_LU_REQUEST;                         /* 设置Message type                         */

        pucSndMsg[usIdx] =
            (VOS_UINT8)(pMsg->MmIeLuType.ucLUT & MM_IE_LU_TYPE_LUT_MASK);       /* 设置Location updating type               */
        pucSndMsg[usIdx] = (VOS_UINT8)(pucSndMsg[usIdx] |
                    (pMsg->MmIeLuType.ucFOR & MM_IE_LU_TYPE_FOR_MASK));         /* 设置The Follow-On Request                */
        pucSndMsg[usIdx] =
            (VOS_UINT8)(pucSndMsg[usIdx] | (pMsg->MmIeCksn.ucCksn << 4));       /* 设置CKSN                                 */
        usIdx++;

        pucSndMsg[usIdx]   |=
            (VOS_UINT8)(0x0000000f & pMsg->MmIeLAI.IeLai.PlmnId.ulMcc);
        pucSndMsg[usIdx++] |=
            (VOS_UINT8)((0x00000f00 & pMsg->MmIeLAI.IeLai.PlmnId.ulMcc) >> 4);
        pucSndMsg[usIdx]   |=
            (VOS_UINT8)((0x000f0000 & pMsg->MmIeLAI.IeLai.PlmnId.ulMcc) >> 16);
        pucSndMsg[usIdx++] |=
            (VOS_UINT8)((0x000f0000 & pMsg->MmIeLAI.IeLai.PlmnId.ulMnc) >> 12);
        pucSndMsg[usIdx]   |=
            (VOS_UINT8)(0x0000000f & pMsg->MmIeLAI.IeLai.PlmnId.ulMnc );
        pucSndMsg[usIdx++] |=
            (VOS_UINT8)((0x00000f00 & pMsg->MmIeLAI.IeLai.PlmnId.ulMnc) >> 4 );
        pucSndMsg[usIdx++] =
            (VOS_UINT8)( (0x0000ff00 & pMsg->MmIeLAI.IeLai.ulLac) >> 8 );
        pucSndMsg[usIdx++] =
            (VOS_UINT8)( 0x000000ff & pMsg->MmIeLAI.IeLai.ulLac );

        /* 设置Mobile station classmark1             */
        MM_Fill_IE_ClassMark1(&pucSndMsg[usIdx++]);

        usMsgSize = usIdx;                                                      /* 调整消息实际长度                         */

        if ( MM_TRUE == Mm_ComMsgSetMobileId (pucSndMsg, &usMsgSize,
                                                &pMsg->MmIeMobileId.MobileID) )
        {
            if ( (NAS_MML_NET_RAT_TYPE_WCDMA == NAS_MML_GetCurrNetRatType() )
              && (MM_TRUE == pMsg->ucMsClsMskFrUMTSFlg) )
            {
                usIdx = usMsgSize;
                usMsgSize = usMsgSize + 5;
                pucSndMsg[usIdx++] = 0x33;
                pucSndMsg[usIdx++] = pMsg->MmIeMsClsMskFrUMTS.aucClassmark2[0];
                for( i = 0; i < pMsg->MmIeMsClsMskFrUMTS.aucClassmark2[0]; i++ )
                {
                    pucSndMsg[usIdx++] =
                        pMsg->MmIeMsClsMskFrUMTS.aucClassmark2[i + 1];
                }
            }


#if   (FEATURE_ON == FEATURE_LTE)

            /* 根据当前是否在CSFB MT流程，封装Additional update parameters IE */
            if ( NAS_MML_CSFB_SERVICE_STATUS_MT_EXIST == NAS_MML_GetCsfbServiceStatus() )
            {
                usIdx = usMsgSize;

                /* 设置CS fallback mobile terminating call */
                pucSndMsg[usIdx]  = MM_IE_CSFB_CSMT_FLAG_EXIST;

                /* 设置ADDITIONAL UPDATE PARAMETERS IEI */
                pucSndMsg[usIdx] |= MM_IEI_ADDITIONAL_UPDATE_PARAMETERS;
                usIdx++;

                /* 消息长度增加 */
                usMsgSize++;
            }
#endif

            Mm_SndRrEstReq( RRC_EST_CAUSE_REGISTRATION, MM_FALSE,
                                                usMsgSize, pucSndMsg );
        }
        else
        {
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuReqSndOfEstReq:WARNING: MSG SET MOBILE ID ERROR!");
        }
        MM_MEM_FREE( VOS_MEMPOOL_INDEX_MM, pucSndMsg);                          /* 释放为发送消息申请的内存空间             */
    }
    return;                                                                     /* 返回                                     */
}
MM_DECODE_RESULT_ENUM_U32 NAS_MM_DecodeMobileId(
    VOS_UINT16          *pusIndex,
    VOS_UINT8           *pucRcvMsg,
    VOS_UINT16          usMsgSize,
    VOS_UINT8           *pucMobileIdFlg,
    VOS_UINT8           *pucEventId)
{
    VOS_UINT16          usIndex;

    usIndex         = *pusIndex;

    /* 消息中包含IE类型，所以增加类型的长度 */
    usIndex++;

    /* 判断消息中是否包含长度字节信息 */
    if ( usMsgSize < (usIndex + 1) )
    {
        return MM_DECODE_END;
    }
    if (( 0 == pucRcvMsg[usIndex] ) || ( 9 < pucRcvMsg[usIndex] ))
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuAccptRcv:WARNING: MOBILE IDENTITY LENGTH Abnormal");
        *pucEventId = MM_EVENT_ID_STATUS_95;
        return MM_DECODE_END;
    }

    /* 判断IE中是否包含mobile identity的完整信息 */
    if ( usMsgSize < (usIndex + pucRcvMsg[usIndex] + 1 ))
    {
        /* 不包含完整信息时，判断IE中是否包含mobile identity中type of identity信息，
           此处的判断是为了GCF用例 26.7.3.2。如果包含IMSI信息，需要删除原来的TMSI */
        if (usMsgSize >= (usIndex + 2))
        {
            if (1 == (pucRcvMsg[usIndex + 1] & MM_IE_MI_ID_TYPE_MASK))
            {
                g_MmMsgLuAccpt.ucMobileIDFlg            = MM_TRUE;
                g_MmMsgLuAccpt.MmIeMobileId.MobileID.ucMsIdFlg
                        = MM_MS_ID_IMSI_PRESENT;
            }
        }
        return MM_DECODE_END;
    }
    if ( MM_FALSE == (*pucMobileIdFlg) )
    {
        *pucMobileIdFlg = MM_TRUE;

        g_MmMsgLuAccpt.ucMobileIDFlg            = MM_TRUE;
        if ( MM_IE_MI_ID_TYPE_IMSI == (pucRcvMsg[usIndex + 1] & 0x07) )
        {
            g_MmMsgLuAccpt.MmIeMobileId.MobileID.ucMsIdFlg
                                   = MM_MS_ID_IMSI_PRESENT;
        }
        else if ( MM_IE_MI_ID_TYPE_TMSI == (pucRcvMsg[usIndex + 1] & 0x07) )
        {
            g_MmMsgLuAccpt.MmIeMobileId.MobileID.ucMsIdFlg
                                       = MM_MS_ID_TMSI_PRESENT;
            PS_MEM_CPY(
                    g_MmMsgLuAccpt.MmIeMobileId.MobileID.aucTmsi,
                    &pucRcvMsg[usIndex + 2],
                    4);
        }
        else
        {
        }
        usIndex = usIndex + pucRcvMsg[usIndex] + 1;
    }
    else
    {
        usIndex = usIndex + pucRcvMsg[usIndex] + 1;
    }

    *pusIndex = usIndex;

    return MM_DECODE_SUCCESS;
}


MM_DECODE_RESULT_ENUM_U32 NAS_MM_DecodeEplmn(
    VOS_UINT16          *pusIndex,
    VOS_UINT8           *pucRcvMsg,
    VOS_UINT16          usMsgSize,
    VOS_UINT8           *pucEventId)
{
    VOS_UINT16          usIndex;
    VOS_UINT16          usEPLmnLen;
    VOS_UINT8           ucPlmnNum;
    VOS_UINT8           i;

    usIndex         = *pusIndex;

    if ( usMsgSize < (usIndex + 2) )
    {
        return MM_DECODE_END;
    }

    if ( pucRcvMsg[usIndex + 1] < 3 )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuAccptRcv:WARNING: RCV MM LU ACCEPT MSG WITH PLMN LIST SIZE ERROR!");
        return MM_DECODE_END;
    }

    if ( usMsgSize < (usIndex + pucRcvMsg[usIndex + 1] + 2) )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuAccptRcv:WARNING: RCV MM LU ACCEPT MSG WITH PLMN LIST SIZE ERROR!");
        return MM_DECODE_END;
    }

    *pusIndex = *pusIndex + pucRcvMsg[usIndex + 1] + 2;

    if ( MM_FALSE == g_MmMsgLuAccpt.ucPlmnListFlg )
    {
        g_MmMsgLuAccpt.ucPlmnListFlg = MM_TRUE;
        usIndex++;
        usEPLmnLen = pucRcvMsg[usIndex];

        ucPlmnNum = pucRcvMsg[usIndex] / 3;
        if (ucPlmnNum > NAS_MAX_EQUPLMN_NUM)
        {
            ucPlmnNum = NAS_MAX_EQUPLMN_NUM;
        }

        g_MmMsgLuAccpt.MmIePlmnList.ucPlmnNum = ucPlmnNum;
        usIndex++;
        for ( i = 0; i < ucPlmnNum; i++ )
        {
            g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMcc
                = pucRcvMsg[usIndex + 1] &
                 MM_IE_LU_ID_MCC_DIGIT_3_MASK;                      /* 存储LAI中的 MCC digit 3                  */
            g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMcc
                = (g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMcc << 8)
                | (pucRcvMsg[usIndex] >> 4);                        /* 存储LAI中的 MCC digit 2                  */
            g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMcc
                = (g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMcc << 8)
                                 | (pucRcvMsg[usIndex] &
                                 MM_IE_LU_ID_MCC_DIGIT_1_MASK);     /* 存储LAI中的 MCC digit 1                  */

            g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMnc
                = pucRcvMsg[usIndex + 1] >> 4;                      /* 存储LAI中的 MNC digit 3                  */
            g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMnc
                = (g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMnc << 8)
                | (pucRcvMsg[usIndex + 2] >> 4);                    /* 存储LAI中的 MNC digit 2                  */
            g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMnc
                = (g_MmMsgLuAccpt.MmIePlmnList.aPlmnId[i].ulMnc << 8)
                                 | (pucRcvMsg[usIndex + 2] &
                                 MM_IE_LU_ID_MNC_DIGIT_1_MASK);
            usIndex = usIndex + 3;
        }
        if (usEPLmnLen < (NAS_MAX_EQUPLMN_NUM*3))
        {
            usIndex = usIndex + (usEPLmnLen%3);
        }
        else
        {
        }
    }



    return MM_DECODE_SUCCESS;
}
MM_DECODE_RESULT_ENUM_U32 NAS_MM_DecodeOtherOptionalIE(
    VOS_UINT16          *pusIndex,
    VOS_UINT8           *pucRcvMsg,
    VOS_UINT16          usMsgSize,
    VOS_UINT8           *pucEventId)
{
    VOS_UINT16          usIndex;

    usIndex         = *pusIndex;

    if (0x0 == (pucRcvMsg[usIndex] & 0xF0))
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuAccptRcv:WARNING: Unexpected IE type.");
        *pucEventId = MM_EVENT_ID_STATUS_96;
        return MM_DECODE_END;
    }

    else if (0x13 == pucRcvMsg[usIndex])
    {/* not code as comprehension required IEI but as LAI IEI in other MM protocal */
        if (usMsgSize < (usIndex + 1))
        {
            return MM_DECODE_END;
        }
        usIndex = usIndex + pucRcvMsg[usIndex+1] + 2;
    }
    else
    {/* not code as comprehension required IEI */
        usIndex = usIndex + 1;
    }

    *pusIndex = usIndex;

    return MM_DECODE_SUCCESS;
}


MM_DECODE_RESULT_ENUM_U32 NAS_MM_DecodeFollowOn(
    VOS_UINT16          *pusIndex,
    VOS_UINT8           *pucRcvMsg,
    VOS_UINT16          usMsgSize)
{
    VOS_UINT16          usIndex;

    usIndex         = *pusIndex;

    if ( MM_FALSE == g_MmMsgLuAccpt.ucFollowOFlg )
    {
        g_MmMsgLuAccpt.ucFollowOFlg = MM_TRUE;
    }
    usIndex++;

    *pusIndex = usIndex;

    return MM_DECODE_SUCCESS;
}


MM_DECODE_RESULT_ENUM_U32 NAS_MM_DecodeCTS(
    VOS_UINT16          *pusIndex,
    VOS_UINT8           *pucRcvMsg,
    VOS_UINT16          usMsgSize)
{
    VOS_UINT16          usIndex;

    usIndex         = *pusIndex;

    if ( MM_FALSE == g_MmMsgLuAccpt.ucCtsPermisFlg )
    {
        g_MmMsgLuAccpt.ucCtsPermisFlg = MM_TRUE;
    }
    usIndex++;

    *pusIndex = usIndex;

    return MM_DECODE_SUCCESS;
}


VOS_UINT8 Mm_ComMsgLuAccptRcv (
                            VOS_UINT8      *pucRcvMsg,                              /* 当前处理的消息                           */
                            VOS_UINT16      usMsgSize                               /* 消息长度,以字节为单位                    */
                            )
{
    VOS_UINT8       ucEventId;                              /* 事件ID                                   */
    VOS_UINT16      usIndex;
    VOS_UINT8       ucMobileIdFlg;
    VOS_UINT32      ulResult;

    usIndex         = 2;
    ucEventId       = MM_EVENT_ID_LU_ACCEPT;
    ucMobileIdFlg   = MM_FALSE;

    PS_MEM_SET( &g_MmMsgLuAccpt, 0, sizeof(MM_MSG_LU_ACCPT_STRU));             /* 清零                                     */

    if (VOS_NULL_PTR == pucRcvMsg)
    {
        ucEventId = MM_EVENT_ID_INVALID;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuAccptRcv:WARNING: RCV VOS_NULL_PTR MM LU ACCEPT MSG!");
    }
    else if (usMsgSize < 7)
    {
        ucEventId = MM_EVENT_ID_STATUS_96;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuAccptRcv:WARNING: RCV MM LU ACCEPT MSG WITH ERROR SIZE!");
    }
    else
    {
        /* 收到Accept消息时需要重置 stChangeRegRejTo17CounterInfo */
        NAS_MM_ResetHplmnRejCauseChangedCounter();
        if ( MM_FALSE == MM_IeChkLai( &pucRcvMsg[usIndex] ) )
        {
            PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuAccptRcv:WARNING: RCV MM LU ACCEPT MSG WITH LAI ERROR!");
            g_MmMsgLuAccpt.MmIeLAI.IeLai.ulLac = MM_INVALID_LAI;                /* plmn非法*/
        }
        else
        {
            Mm_ComSaveLai( &pucRcvMsg[usIndex], &g_MmMsgLuAccpt.MmIeLAI.IeLai );
        }

        usIndex = usIndex + 5;

        g_MmMsgLuAccpt.ucFollowOFlg             = MM_FALSE;
        g_MmMsgLuAccpt.ucCtsPermisFlg           = MM_FALSE;
        g_MmMsgLuAccpt.ucMobileIDFlg            = MM_FALSE;
        g_MmMsgLuAccpt.ucPlmnListFlg            = MM_FALSE;
        g_MmMsgLuAccpt.ucEmcFlg                 = MM_FALSE;
        Mm_TimerStop(MM_TIMER_T3218);                                           /* 停T3218                                  */
        Mm_ComDelRandRes();                                                     /* 删除RAND和RES                            */

        while ( usIndex < usMsgSize )
        {
            if ( 0x17 == pucRcvMsg[usIndex] )
            {                                                                   /* 解码mobile identity */
                ulResult = NAS_MM_DecodeMobileId(&usIndex, pucRcvMsg, usMsgSize,
                                &ucMobileIdFlg, &ucEventId);
            }
            else if ( 0xa1 == pucRcvMsg[usIndex] )
            {                                                                   /* 解码follow on */
                ulResult = NAS_MM_DecodeFollowOn(&usIndex, pucRcvMsg, usMsgSize);
            }
            else if ( 0xa2 == pucRcvMsg[usIndex] )
            {                                                                   /* 解码CTS */
                ulResult = NAS_MM_DecodeCTS(&usIndex, pucRcvMsg, usMsgSize);
            }
            else if ( 0x4a == pucRcvMsg[usIndex] )
            {                                                                   /* 解码EPLMN */
                ulResult = NAS_MM_DecodeEplmn(&usIndex, pucRcvMsg,
                                usMsgSize, &ucEventId);
            }
            else if (0x34 == pucRcvMsg[usIndex])
            {                                                                   /* Emergency Number List                    */

                NAS_MML_DecodeEmergencyNumList(&usIndex, pucRcvMsg, usMsgSize);

                g_MmMsgLuAccpt.ucEmcFlg = MM_TRUE;
                
                ulResult = MM_DECODE_SUCCESS;
            }
            else
            {                                                                   /* 解码其它可选的IE */
                ulResult = NAS_MM_DecodeOtherOptionalIE(&usIndex, pucRcvMsg,
                                usMsgSize, &ucEventId);
            }

            if (MM_DECODE_END == ulResult)
            {
                break;
            }
        }
    }
    return ucEventId;                                                           /* 返回事件ID                               */
}

/*******************************************************************************
  Module:   Mm_ComMsgLuRejctRcv
  Function: 存储消息LU ACCEPT
  Input:    VOS_VOID     *pMsg,  当前处理的消息
            VOS_UINT16    usMsgSize, 消息长度,以字节为单位
  Output:
  NOTE:
  Return:   VOS_UINT8,事件ID,详见MM_Def.h中关于事件的宏定义
  History:
      1.  张志勇    2004.03.25   新版作成
      2. 日    期   : 2007年05月11日
         作    者   : luojian id:60022475
         修改内容   : 问题单号:A32D10713
*******************************************************************************/
VOS_UINT8 Mm_ComMsgLuRejctRcv (
                            VOS_UINT8      *pucRcvMsg,                              /* 当前处理的消息                           */
                            VOS_UINT16      usMsgSize                               /* 消息长度,以字节为单位                    */
                            )
{
    VOS_UINT8       ucEventId = MM_EVENT_ID_LU_REJECT;                              /* 事件ID                                   */

    PS_MEM_SET( &g_MmMsgLuRjct, 0, sizeof(MM_MSG_LU_RJCT_STRU));               /* 清零                                     */

    if (VOS_NULL_PTR == pucRcvMsg)
    {
        ucEventId = MM_EVENT_ID_INVALID;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuRejctRcv:WARNING: RCV VOS_NULL_PTR MM LU REJECT MSG!");
    }
    else if (usMsgSize < 3)
    {
        ucEventId = MM_EVENT_ID_STATUS_96;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgLuRejctRcv:WARNING: RCV MM LU REJECT MSG WITH ERROR SIZE!");
    }
    else
    {
        g_MmMsgLuRjct.MmIeRejCause.ucRejCause = pucRcvMsg[2];
    }

    g_MmGlobalInfo.LuInfo.ucT3212ExpiredFlg = MM_FALSE;

    return ucEventId;                                                           /* 返回事件ID                               */
}


/*******************************************************************************
  Module:   Mm_ComMsgSvcPromptRcv
  Function: 存储消息LU ACCEPT
  Input:    VOS_VOID     *pMsg,  当前处理的消息
  Output:
  NOTE:
  Return:   VOS_UINT8,事件ID,详见MM_Def.h中关于事件的宏定义
  History:
      1.  张志勇      2004.04.14   新版作成
*******************************************************************************/
VOS_UINT8 Mm_ComMsgSvcPromptRcv (
                            VOS_UINT8      *pucRcvMsg,                              /* 当前处理的消息                           */
                            VOS_UINT16      usMsgSize                               /* 消息长度,以字节为单位                    */
                            )
{
    VOS_UINT8       ucEventId = MM_EVENT_ID_CM_SVC_PROMPT;                          /* 事件ID                                   */

    if (VOS_NULL_PTR == pucRcvMsg)
    {
        ucEventId = MM_EVENT_ID_INVALID;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgSvcPromptRcv:WARNING: RCV VOS_NULL_PTR MM CM SVC PROMPT MSG!");
        return ucEventId;
    }
    else if (usMsgSize < 3)
    {
        ucEventId = MM_EVENT_ID_STATUS_96;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgSvcPromptRcv:WARNING: RCV MM CM SVC PROMPT MSG WITH ERROR SIZE!");
        return ucEventId;
    }
    else
    {

    }

    if ( MM_FALSE == MM_IeChkPdAndSapi( &pucRcvMsg[2] ) )
    {
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgSvcPromptRcv:WARNING: RCV MM CM SVC PROMPT MSG WITH PD AND SAPI ERROR!");
        return MM_EVENT_ID_STATUS_95;
    }

    if ( MM_IE_PD_CALL_CONTROL != ( pucRcvMsg[2] & 0x0f ) )
    {                                                                           /* PD不合法                                 */
        ucEventId = MM_EVENT_ID_INVALID;
        PS_LOG(WUEPS_PID_MM, VOS_NULL, PS_PRINT_WARNING, "Mm_ComMsgSvcPromptRcv:WARNING: RCV MM CM SVC PROMPT MSG WITH PD ERROR!");
    }

    return ucEventId;                                                           /* 返回事件ID                               */
}
/*******************************************************************************
  Module:   MM_IeChkLai
  Function: 检查IE LAI的正确性
  Input:    VOS_UINT8      *pucRcvIe  当前处理的消息
  Output:
  NOTE:
  Return:   MM_TRUE/MM_FALSE
  History:
      1.  张志勇    2005.01.12   新版作成
*******************************************************************************/
VOS_UINT8 MM_IeChkLai (
                   VOS_UINT8      *pucRcvIe                                         /* 当前处理的消息                           */
                   )
{
    VOS_UINT8   ucIndex = 0;

    if( ( pucRcvIe[ucIndex] & 0x0F ) > 9 )
    {
        return MM_FALSE;
    }
    if( ( ( pucRcvIe[ucIndex] >> 4 ) & 0x0F ) > 9 )
    {
        return MM_FALSE;
    }
    if( ( pucRcvIe[ucIndex + 1] & 0x0F ) > 9 )
    {
        return MM_FALSE;
    }
    if( ( ( ( pucRcvIe[ucIndex + 1] >> 4 ) & 0x0F ) > 9 ) &&
        ( ( ( pucRcvIe[ucIndex + 1] >> 4 ) & 0x0F ) != 0x0F ) )
    {
        return MM_FALSE;
    }
    if( ( pucRcvIe[ucIndex + 2] & 0x0F ) > 9 )
    {
        return MM_FALSE;
    }
    if( ( ( pucRcvIe[ucIndex + 2] >> 4 ) & 0x0F ) > 9 )
    {
        return MM_FALSE;
    }
    return MM_TRUE;
}
VOS_UINT8 MM_IeChkMblId (
                   VOS_UINT8      *pucRcvIe                                         /* 当前处理的消息                           */
                   )
{
    VOS_UINT8   ucIndex = 0;
    VOS_UINT8   ucLength = 0;

    if( ( pucRcvIe[ucIndex + 1] & MM_IE_MI_ID_TYPE_MASK ) > 4 )
    {
        return MM_FALSE;
    }
    if ( ( MM_IE_MI_ID_TYPE_IMSI == ( pucRcvIe[ucIndex + 1] & \
     MM_IE_MI_ID_TYPE_MASK ) ) ||
       ( MM_IE_MI_ID_TYPE_IMEI == ( pucRcvIe[ucIndex + 1] &\
        MM_IE_MI_ID_TYPE_MASK ) ) ||
       ( MM_IE_MI_ID_TYPE_IMEISV == ( pucRcvIe[ucIndex + 1] & \
        MM_IE_MI_ID_TYPE_MASK ) ) )
    {
        if ( 0 == (pucRcvIe[ucIndex + 1] & 0x08) )
        {
            ucLength = pucRcvIe[ucIndex];
            if ((pucRcvIe[ucLength] & MM_IE_MI_ID_DIGIT_1_MASK ) != 0xF0)
            {
                return MM_FALSE;
            }
        }
    }
    else
    {
        if( ( pucRcvIe[ucIndex + 1] & MM_IE_MI_ID_DIGIT_1_MASK ) != 0xF0 )
        {
            return MM_FALSE;
        }
    }

    return MM_TRUE;
}
/*******************************************************************************
  Module:   MM_IeChkPlmnLst
  Function: 检查IE PLMN LIST的正确性
  Input:    VOS_UINT8      *pucRcvIe  当前处理的消息
  Output:
  NOTE:
  Return:   MM_TRUE/MM_FALSE
  History:
      1.  张志勇    2005.01.13   新版作成
*******************************************************************************/
VOS_UINT8 MM_IeChkPlmnLst (
                   VOS_UINT8      *pucRcvIe                                         /* 当前处理的消息                           */
                   )
{
    VOS_UINT8   ucIndex = 0;
    VOS_UINT8   ucPlmnNum;

    if ( 0 != ( pucRcvIe[ucIndex] % 3 ) )
    {
        return MM_FALSE;
    }
    else
    {
        ucPlmnNum = pucRcvIe[ucIndex] / 3;
        if (ucPlmnNum > 15)
        {
            return MM_FALSE;
        }
    }

    return MM_TRUE;
}

/*******************************************************************************
  Module:   MM_IeChkCksn
  Function: 检查IE CKSN的正确性
  Input:    VOS_UINT8      *pucRcvIe  当前处理的消息
  Output:
  NOTE:
  Return:   MM_TRUE/MM_FALSE
  History:
      1.  张志勇    2005.01.13   新版作成
*******************************************************************************/
VOS_UINT8 MM_IeChkCksn (
                   VOS_UINT8      *pucRcvIe                                         /* 当前处理的消息                           */
                   )
{
    return MM_TRUE;
}


/*******************************************************************************
  Module:   MM_IeChkSprHalfOctet
  Function: 检查IE Spare Half Octet的正确性
  Input:    VOS_UINT8      *pucRcvIe  当前处理的消息
  Output:
  NOTE:
  Return:   MM_TRUE/MM_FALSE
  History:
      1.  张志勇    2005.01.13   新版作成
*******************************************************************************/
VOS_UINT8 MM_IeChkSprHalfOctet (
                   VOS_UINT8      *pucRcvIe                                         /* 当前处理的消息                           */
                   )
{
    if ( 0 != ( pucRcvIe[0] & 0xF0 ) )
    {
        return MM_FALSE;
    }

    return MM_TRUE;
}

/*******************************************************************************
  Module:   MM_IeChkRand
  Function: 检查IE RAND的正确性
  Input:    VOS_UINT8      *pucRcvIe  当前处理的消息
  Output:
  NOTE:
  Return:   MM_TRUE/MM_FALSE
  History:
      1.  张志勇    2005.01.13   新版作成
*******************************************************************************/
VOS_UINT8 MM_IeChkRand (
                   VOS_UINT8      *pucRcvIe                                         /* 当前处理的消息                           */
                   )
{
    return MM_TRUE;
}

/*******************************************************************************
  Module:   MM_IeChkAutn
  Function: 检查IE AUTN的正确性
  Input:    VOS_UINT8      *pucRcvIe  当前处理的消息
            VOS_UINT8      ucflg       是否为可选项
  Output:
  NOTE:
  Return:   MM_TRUE/MM_FALSE
  History:
      1.  张志勇    2005.01.13   新版作成
*******************************************************************************/
VOS_UINT8 MM_IeChkAutn (
                   VOS_UINT8      *pucRcvIe                                         /* 当前处理的消息                           */
                   )
{
    VOS_UINT8       ucIndex = 0;

    if ( 16 != pucRcvIe[ucIndex] )
    {
        return MM_FALSE;
    }
    return MM_TRUE;
}

/*******************************************************************************
  Module:   MM_IeChkIdType
  Function: 检查IE ID TYPE的正确性
  Input:    VOS_UINT8      *pucRcvIe  当前处理的消息
  Output:
  NOTE:
  Return:   MM_TRUE/MM_FALSE
  History:
      1.  张志勇    2005.01.13   新版作成
*******************************************************************************/
VOS_UINT8 MM_IeChkIdType (
                   VOS_UINT8      *pucRcvIe                                         /* 当前处理的消息                           */
                   )
{
    if( ( 1 != ( pucRcvIe[0] & MM_IE_ID_TYPE_IDTYPE_MASK) )&&
        ( 2 != ( pucRcvIe[0] & MM_IE_ID_TYPE_IDTYPE_MASK) )&&
        ( 3 != ( pucRcvIe[0] & MM_IE_ID_TYPE_IDTYPE_MASK) )&&
        ( 4 != ( pucRcvIe[0] & MM_IE_ID_TYPE_IDTYPE_MASK ) ) )
    {
        return MM_FALSE;
    }
    return MM_TRUE;
}

/*******************************************************************************
  Module:   MM_IeChkPdAndSapi
  Function: 对IE"PD and SAPI"检查
  Input:    VOS_UINT8 *pIePdAndSapi,  被检查IE首地址
  Output:
  NOTE:
  Return:   VOS_UINT8,检查结果,MM_TRUE/MM_FALSE,成功/失败
  History:
      1.  张志勇    2005.01.13   新版作成
*******************************************************************************/
VOS_UINT8 MM_IeChkPdAndSapi( VOS_UINT8 *pIePdAndSapi )
{
    VOS_UINT8 ucRet = MM_TRUE;
    VOS_UINT8 ucIndex = 0;
    VOS_UINT8 ucPd,ucSapi;

    ucSapi = ( pIePdAndSapi[ucIndex] & 0x30 ) >> 4;
    ucPd   = ( pIePdAndSapi[ucIndex] & 0x0f );
    if ( (0x00 != ucSapi) && (0x03 != ucSapi) )
    {
        ucRet = MM_FALSE;
    }
    else
    {
        ;
    }
    if ( MM_TRUE == ucRet )
    {
        if (  (0x07 == ucPd)
           || (0x0d == ucPd)
           || (0x0e == ucPd)
           || (0x0f == ucPd)
           )
        {
            ucRet = MM_FALSE;
        }
        else
        {
            ;
        }
    }
    else
    {
        ;
    }
    return ucRet;
}


VOS_UINT8 MM_IeChkDayLightSavTim( VOS_UINT8* pIeDayLightSvTim )
{
    VOS_UINT8 ucRet = MM_TRUE;
    VOS_UINT8 ucIndex = 0;

    if ( 1 == pIeDayLightSvTim[ucIndex] )
    {
        ucIndex ++;
        if( (0x00 != pIeDayLightSvTim[ucIndex])
            && (0x01 != pIeDayLightSvTim[ucIndex])
            && (0x02 != pIeDayLightSvTim[ucIndex])
            )
        {
            ucRet = MM_FALSE;
        }
        else
        {
            g_MmMsgMmInfo.ucNWDyLitSavTmFlg = VOS_TRUE;
            g_MmMsgMmInfo.MmIeNWDyLitSavTm.ucValue = pIeDayLightSvTim[ucIndex];
        }
    }
    else
    {
        ucRet = MM_FALSE;
    }

    return ucRet;
}

/*******************************************************************************
  Module:   MM_IeChkLsaIdtfr
  Function: 对IE"LSA Identifier"检查
  Input:    VOS_UINT8 *pIeLsaIdTfr,  被检查IE首地址
  Output:
  NOTE:
  Return:   VOS_UINT8,检查结果,VOS_TRUE/VOS_FALSE,成功/失败
  History:
      1.  张志勇    2005.01.13   新版作成
*******************************************************************************/
VOS_UINT8 MM_IeChkLsaIdtfr( VOS_UINT8* pIeLsaIdTfr )
{
    VOS_UINT8 ucRet = VOS_TRUE;
    VOS_UINT8 ucIndex = 0;

    if ( pIeLsaIdTfr[ucIndex] <= 3 )
    {
        ;
    }
    else
    {
        ucRet = VOS_FALSE;
    }
    return ucRet;
}


VOS_UINT8 MM_IeChkTimAndTimZone( VOS_UINT8 * pIeTimAndTimZone )
{
    VOS_INT8    cTimeZone;
    VOS_UINT8   ucDigit;

    /*格式转换: 网络下发的格式为BCD码，高字节为个位数，低字节为十位数, 如: 0x51 => 15 */
    /* 第一个字节为 Year */
    g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucYear = ((pIeTimAndTimZone[0] & 0x0f) * 10)
                                                     +((pIeTimAndTimZone[0] & 0xf0) >> 4);
    if ( g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucYear > 99)
    {
        return VOS_FALSE;
    }

    /* 第二个字节为 Month */
    g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucMonth = ((pIeTimAndTimZone[1] & 0x0f) * 10)
                                                     +((pIeTimAndTimZone[1] & 0xf0) >> 4);
    if (  ( g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucMonth > 12 )
        ||( 0 == g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucMonth ) )
    {
        return VOS_FALSE;
    }

    /* 第三个字节为 Day */
    g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucDay = ((pIeTimAndTimZone[2] & 0x0f) * 10)
                                                     +((pIeTimAndTimZone[2] & 0xf0) >> 4);
    if ( ( g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucDay > 31)
      || ( 0 == g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucDay ) )
    {
        return VOS_FALSE;
    }

    /* 第四个字节为 Hour */
    g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucHour = ((pIeTimAndTimZone[3] & 0x0f) * 10)
                                                     +((pIeTimAndTimZone[3] & 0xf0) >> 4);
    if ( g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucHour >= 24)
    {
        return VOS_FALSE;
    }

    /* 第五个字节为 Minute */
    g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucMinute = ((pIeTimAndTimZone[4] & 0x0f) * 10)
                                                     +((pIeTimAndTimZone[4] & 0xf0) >> 4);
    if ( g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucMinute >= 60)
    {
        return VOS_FALSE;
    }

    /* 第六个字节为 Second */
    g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucSecond = ((pIeTimAndTimZone[5] & 0x0f) * 10)
                                                     +((pIeTimAndTimZone[5] & 0xf0) >> 4);
    if ( g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.ucSecond >= 60)
    {
        return VOS_FALSE;
    }

    /* 第七个字节为 TimeZone */
    ucDigit = (pIeTimAndTimZone[6] & 0xF0) >> 4;
    if ( ucDigit > 9 )
    {
        return VOS_FALSE;
    }
    cTimeZone = (VOS_INT8)(((pIeTimAndTimZone[6] & 0x07) * 10) + ucDigit);

    /* 判断时区的正负值 */
    if (pIeTimAndTimZone[6] & 0x08)
    {
        g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.cTimeZone = - cTimeZone;
    }
    else
    {
        g_MmMsgMmInfo.MmIeUnvrslTmAndLocalTmZone.cTimeZone = cTimeZone;
    }

    g_MmMsgMmInfo.ucUnvrslTmAndLocalTmZoneFlg = VOS_TRUE;

    return VOS_TRUE;
}
VOS_UINT8 MM_IeChkTimZone( VOS_UINT8* pIeTimZone )
{
    VOS_INT8    cTimeZone;
    VOS_UINT8   ucDigit;

    ucDigit = ((*pIeTimZone) & 0xF0) >> 4;
    if ( ucDigit > 9 )
    {
        /* TimeZone 值非法 */
        return VOS_FALSE;
    }

    cTimeZone = (VOS_INT8)((((*pIeTimZone) & 0x07) * 10) + ucDigit);

    /* 保存Time Zone */
    g_MmMsgMmInfo.ucLocalTmZoneFlg = VOS_TRUE;

    /* 判断时区的正负值 */
    if ((*pIeTimZone) & 0x08)
    {
        g_MmMsgMmInfo.MmIeLocalTmZone.cTimeZone = - cTimeZone;
    }
    else
    {
        g_MmMsgMmInfo.MmIeLocalTmZone.cTimeZone = cTimeZone;
    }

    return VOS_TRUE;
}


VOS_UINT8 MM_IeChkNtwkNam( VOS_UINT8* pIeNtWkNam )
{
    VOS_UINT8 ucRet = VOS_TRUE;
    VOS_UINT8 ucIndex = 0;
    VOS_UINT8 ucIeLen,ucOctetN,ucCodScheme;

    ucIeLen     = pIeNtWkNam[ucIndex++];
    ucOctetN    = pIeNtWkNam[ucIeLen];
    ucCodScheme = ( 0x70 & pIeNtWkNam[ucIndex] ) >> 4;

    if ( ( (0 == ucCodScheme) || (1 == ucCodScheme) )
        && (0 != (0x80 & pIeNtWkNam[ucIndex])) )
    {
        switch( ( 0x07 & pIeNtWkNam[ucIndex] ) )
        {
        case 0:
            break;
        case 1:
            if ( 0 != ( 0x80 & ucOctetN ) )
            {
                ucRet = VOS_FALSE;
            }
            break;
        case 2:
            if ( 0 != ( 0xc0 & ucOctetN ) )
            {
                ucRet = VOS_FALSE;
            }
            break;
        case 3:
            if ( 0 != ( 0xe0 & ucOctetN ) )
            {
                ucRet = VOS_FALSE;
            }
            break;
        case 4:
            if ( 0 != ( 0xf0 & ucOctetN ) )
            {
                ucRet = VOS_FALSE;
            }
            break;
        case 5:
            if ( 0 != ( 0xf8 & ucOctetN ) )
            {
                ucRet = VOS_FALSE;
            }
            break;
        case 6:
            if ( 0 != ( 0xfc & ucOctetN ) )
            {
                ucRet = VOS_FALSE;
            }
            break;
        case 7:
            if ( 0 != ( 0xfe & ucOctetN ) )
            {
                ucRet = VOS_FALSE;
            }
            break;
        default:
            break;
        }
    }
    else
    {
        ucRet = VOS_FALSE;
    }

    return ucRet;
}

VOS_UINT32 NAS_MM_CheckSigConnStatusAffectAttach(
    MMCMM_ATTACH_REQ_STRU             *pstAttachReq
)
{
    if(((MM_COM_SRVST_NORMAL_SERVICE == Gmm_ComServiceSts())
      && (MM_NORMAL_SERVICE == g_MmGlobalInfo.ucMmServiceState ))
    && (MMC_MM_ATTACH_TYPE_GPRS_IMSI == pstAttachReq->enAttachType)
    && ((GMM_SPECIFIC_ON == g_MmSubLyrShare.GmmShare.ucPsSpecificFlg)
     || (MM_SPECIFIC_ON     == g_MmSubLyrShare.MmShare.ucCsSpecificFlg)))
    {
        return VOS_TRUE;
    }


    if (( MMC_MM_ATTACH_TYPE_IMSI == pstAttachReq->enAttachType )
     && (MM_NORMAL_SERVICE == g_MmGlobalInfo.ucMmServiceState)
     && (MM_SPECIFIC_ON     == g_MmSubLyrShare.MmShare.ucCsSpecificFlg))
    {
        return VOS_TRUE;
    }

    /* CS当前在专有过程中,直接返回注册结果 */
    if ((( MMC_MM_ATTACH_TYPE_IMSI == pstAttachReq->enAttachType )
     || (MMC_MM_ATTACH_TYPE_GPRS_IMSI == pstAttachReq->enAttachType))
     && (MM_SPECIFIC_ON     == g_MmSubLyrShare.MmShare.ucCsSpecificFlg))
    {
        NAS_MML_SetCsAttachAllowFlg(VOS_TRUE);
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT32 NAS_MM_CheckSimStatusBeforeAttachAction(
    MMCMM_ATTACH_REQ_STRU             *pstAttachReq
)
{
    NAS_MML_SIM_STATUS_STRU            *pstUsimStatus = VOS_NULL_PTR;

    pstUsimStatus = NAS_MML_GetSimStatus();

    /* USIM CS域无效，直接回复ATTACH CNF */
    if ( ( MMC_MM_ATTACH_TYPE_GPRS  != pstAttachReq->enAttachType )
      && ( VOS_TRUE            != pstUsimStatus->ucSimCsRegStatus ) )
    {
        NAS_NORMAL_LOG(WUEPS_PID_GMM, "NAS_GMM_CheckSimStatusAffectAttach:WARNING: CS USIM is invalid.");
        NAS_MM_SndMmcAttachCnf();
        return VOS_TRUE;
    }

    /* USIM PS域无效，MMC_GMM_ATTACH_TYPE_GPRS_IMSI类型转换为MMC_MM_ATTACH_TYPE_IMSI */
    if ( ( MMC_MM_ATTACH_TYPE_GPRS_IMSI == pstAttachReq->enAttachType )
      && ( VOS_TRUE                != pstUsimStatus->ucSimPsRegStatus ) )
    {
        pstAttachReq->enAttachType = MMC_MM_ATTACH_TYPE_IMSI;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MM_ConflictWithDetach(VOS_UINT32 ulAttachType)
{
    /* 当前MM的detach标志位为VOS_TRUE，ATTACH类型是MMC_MM_ATTACH_TYPE_IMSI
       或者MMC_MM_ATTACH_TYPE_GPRSIMSI ,返回VOS_TRUE */
    NAS_MML_CAMP_PLMN_INFO_STRU        *pstCampInfo = VOS_NULL_PTR;

    pstCampInfo = NAS_MML_GetCurrCampPlmnInfo();

    /* 之前不存在DETACH流程 */
    if ( MM_WAIT_NULL_DETACH == g_MmGlobalInfo.stDetachInfo.enDetachType)
    {
        return VOS_FALSE;
    }

    /* A+I模式 */
    if ( ( NAS_MML_MS_MODE_PS_CS == NAS_MML_GetMsMode() )
      && ( NAS_MML_NET_MODE_I    == pstCampInfo->enNetworkMode ) )
    {
        /* 网络模式I下,存DETACH流程,直接回复CNF */
        if ( MMC_MM_ATTACH_TYPE_IMSI == (ulAttachType
                                        & MMC_MM_ATTACH_TYPE_IMSI))
        {
            NAS_MM_SndMmcAttachCnf();
        }
        return VOS_TRUE;
    }
    else
    {
        if ( ( (MMC_MM_ATTACH_TYPE_IMSI == ulAttachType )
             && (MM_WAIT_CS_DETACH == (g_MmGlobalInfo.stDetachInfo.enDetachType
                                     & GMM_WAIT_CS_DETACH ) ) )
          || ( MMC_MM_ATTACH_TYPE_GPRS_IMSI == ulAttachType ) )
        {
            /* CS ATTACH 和DETACH冲突,直接回复CNF */
            NAS_MM_SndMmcAttachCnf();
            return VOS_TRUE;
        }
    }

    return VOS_FALSE;
}

VOS_UINT32 NAS_MM_IsAttachRepeated(
    VOS_UINT32                          ulAttachType,
    VOS_UINT32                          ulOpID)
{
    /* 之前存在CS ATTACH 收到CS ATTACH，更新OPID后直接返回 */
    if (  MM_WAIT_CS_ATTACH == (g_MmGlobalInfo.stAttachInfo.enAttachType
                              & MM_WAIT_CS_ATTACH))
    {
        g_MmGlobalInfo.stAttachInfo.ulOpid = ulOpID;
        return VOS_TRUE;
    }

    return VOS_FALSE;
}
VOS_UINT8 NAS_MM_RcvMmcAttachReq_PreProc(
    VOS_VOID                           *pstRcvMsg
)
{
    MMCMM_ATTACH_REQ_STRU              *pstAttachReq;
    NAS_MML_CAMP_PLMN_INFO_STRU        *pstCampInfo;

    pstAttachReq    = (MMCMM_ATTACH_REQ_STRU*)pstRcvMsg;
    pstCampInfo     = NAS_MML_GetCurrCampPlmnInfo();

    /* 判定是否需要处理该消息，无效消息返函数返回MM_EVENT_ID_INVALID */
    if ((MMC_MM_ATTACH_TYPE_IMSI     != pstAttachReq->enAttachType)
     && (MMC_MM_ATTACH_TYPE_GPRS_IMSI!= pstAttachReq->enAttachType))
    {

        NAS_WARNING_LOG(WUEPS_PID_GMM, "NAS_MM_RcvMmcAttachReq_PreProc:ATTACH TYPE INVALID");
        return MM_EVENT_ID_INVALID;
    }

    /* PS非接入禁止的情况,需要清除LIKEB标记，避免重复发起LAU */
    if (MM_TRUE == g_MmGlobalInfo.ucSysInfoFlg)
    {
        if ((VOS_TRUE == NAS_MML_GetPsAttachAllowFlg())
         && (MM_NET_MODE_I == g_MmGlobalInfo.ucNtMod))
        {
            if ((VOS_TRUE == NAS_MML_GetSimPsRegStatus())
             && (MM_UNSUPPORT_GPRS != g_MmGlobalInfo.usRac))
            {
                if((VOS_TRUE == NAS_MML_GetPsAttachAllowFlg())
                || (MMC_MM_ATTACH_TYPE_GPRS_IMSI == pstAttachReq->enAttachType))
                {
                    g_MmGlobalInfo.ucLikeB = MM_FALSE;
                }
            }
        }
    }


    g_MmGlobalInfo.stAttachInfo.ulOpid = pstAttachReq->ulOpid;

    if (VOS_TRUE == NAS_MM_CheckSigConnStatusAffectAttach(pstAttachReq))
    {
        NAS_MM_SndMmcAttachCnf();
        return MM_EVENT_ID_INVALID;
    }

    /* CS域卡无效，回复ATTACH_CNF，返回MM_EVENT_ID_INVALID */
    if (VOS_TRUE == NAS_MM_CheckSimStatusBeforeAttachAction(pstAttachReq))
    {
        return MM_EVENT_ID_INVALID;
    }

    /* 检查是否和之前的DETACH有冲突 */
    if (VOS_TRUE == NAS_MM_ConflictWithDetach(pstAttachReq->enAttachType))
    {
        return MM_EVENT_ID_INVALID;
    }

    /* 检查是否和之前的ATTACH有冲突 */
    if (VOS_TRUE == NAS_MM_IsAttachRepeated(pstAttachReq->enAttachType, pstAttachReq->ulOpid))
    {
        return MM_EVENT_ID_INVALID;
    }

    /* 网络模式II下更改ATTACH类型 */
    if ((NAS_MML_NET_MODE_II            == pstCampInfo->enNetworkMode)
     && (MMC_MM_ATTACH_TYPE_GPRS_IMSI   == pstAttachReq->enAttachType))
    {
        pstAttachReq->enAttachType = MMC_MM_ATTACH_TYPE_IMSI;
    }

    /* 如果当前ucCsAttachState的状态已经附着，则回复ATTACH_CNF，
       返回MM_EVENT_ID_INVALID */
    if ( MM_STATUS_ATTACHED == g_MmSubLyrShare.MmShare.ucCsAttachState )
    {
        NAS_MM_SndMmcAttachCnf();
        return MM_EVENT_ID_INVALID;
    }

    /* 设置等待ATTACH结果标志位，返回MM_EVENT_ID_ATTACH_REQ */
    g_MmGlobalInfo.stAttachInfo.enAttachType    = pstAttachReq->enAttachType;

    /* 置为CsAttachAllowFlg 为VOS_TRUE */
    NAS_MML_SetCsAttachAllowFlg(VOS_TRUE);

    return MM_EVENT_ID_ATTACH_REQ;
}


VOS_VOID NAS_MM_RcvAttachReq_InterRatChange(
    VOS_VOID                          *pMsg
)
{
    /* 直接回复MMCMM_ATTACH_CNF */
    NAS_MM_SndMmcAttachCnf();

    /* 清除等待ATTACH结果标志位 */
    g_MmGlobalInfo.stAttachInfo.enAttachType    = MM_WAIT_NULL_ATTACH;

    /* 置为CsAttachAllowFlg 为VOS_TRUE */
    NAS_MML_SetCsAttachAllowFlg(VOS_TRUE);
}

VOS_UINT32 NAS_MM_ProcCsDetach(
    MMCMM_DETACH_REQ_STRU              *pstDetachReq
)
{
    VOS_UINT32                          ulState;

    /* 获取MM的状态 */
    ulState = Mm_GetState();



    g_MmGlobalInfo.stDetachInfo.ulOpid = pstDetachReq->ulOpid;


    NAS_MM_InitLaiInfo(NAS_MM_GetAttemptUpdateLaiInfo());

    /* CS域有当前业务存在,syscfg设置引起的detach操作，不缓存，直接回复detach cnf带当前服务状态 */
    if ( VOS_TRUE == NAS_MML_GetCsServiceExistFlg() )
    {
        if (VOS_TRUE == NAS_MMA_GetServiceDomainSet())
        {
            NAS_MM_SndMmcDetachCnf(g_MmGlobalInfo.ucMmServiceState);
            return VOS_TRUE;
        }
    }

    if ( MM_WAIT_CS_ATTACH  == (g_MmGlobalInfo.stAttachInfo.enAttachType & MM_WAIT_CS_ATTACH) )
    {
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_ProcCsDetach:NORMAL: CS ATTACH is Running.");
        NAS_MM_SndMmcAttachCnf();

        g_MmGlobalInfo.stAttachInfo.enAttachType &= ~MM_WAIT_CS_ATTACH;
    }

    /* 非网络模式I下,在已注册或者LAU过程中，处理CS域的detach;
       网络模式I下,  在已注册或者联合注册过程中或者LAU过程中，处理CS域的detach  */
    if ( (MM_STATUS_ATTACHED                       == g_MmSubLyrShare.MmShare.ucCsAttachState)
      || (LOCATION_UPDATING_PENDING                == ulState)
      || (WAIT_FOR_RR_CONNECTION_LOCATION_UPDATING == ulState)
      || (LOCATION_UPDATING_INITIATED              == ulState))
    {
        g_MmGlobalInfo.stDetachInfo.enDetachType |= MM_WAIT_CS_DETACH;
    }
    else
    {
        g_MmGlobalInfo.stDetachInfo.enDetachType |= MM_WAIT_CS_DETACH;
        
        NAS_MM_LocalDetach();


        return VOS_TRUE;
    }

    return VOS_FALSE;
}


VOS_UINT32 NAS_MM_ProcCsPsDetach(
    MMCMM_DETACH_REQ_STRU              *pstDetachReq
)
{
    VOS_UINT32                          ulState;

    /* 获取MM的状态 */
    ulState = Mm_GetState();


    g_MmGlobalInfo.stDetachInfo.ulOpid = pstDetachReq->ulOpid;


    NAS_MM_InitLaiInfo(NAS_MM_GetAttemptUpdateLaiInfo());

    
    /* 当前存在CS域ATTACH标志，则先直接回复ATTACH CNF结果 */
    if ( MM_WAIT_CS_ATTACH  == (g_MmGlobalInfo.stAttachInfo.enAttachType & MM_WAIT_CS_ATTACH) )
    {
        NAS_NORMAL_LOG(WUEPS_PID_MM, "NAS_MM_ProcCsPsDetach:NORMAL: CS ATTACH is Running.");

        NAS_MM_SndMmcAttachCnf();

        g_MmGlobalInfo.stAttachInfo.enAttachType &= ~MM_WAIT_CS_ATTACH;
    }

    /* 清楚ATTACH等待标志 */
    g_MmGlobalInfo.stAttachInfo.enAttachType = MM_WAIT_NULL_ATTACH;

    /* CS处于ATTACHED状态保留标志位,或者在联合注册过程中, 否则直接回复DETACH CNF*/
    if ( (MM_STATUS_ATTACHED                       == g_MmSubLyrShare.MmShare.ucCsAttachState)
      || (LOCATION_UPDATING_PENDING                == ulState)
      || (WAIT_FOR_RR_CONNECTION_LOCATION_UPDATING == ulState)
      || (LOCATION_UPDATING_INITIATED              == ulState))
    {
        g_MmGlobalInfo.stDetachInfo.enDetachType    = MM_WAIT_CS_PS_DETACH;

        return VOS_FALSE;
    }
    else
    {
        g_MmGlobalInfo.stDetachInfo.enDetachType |= MM_WAIT_CS_PS_DETACH;
        
        NAS_MM_LocalDetach();
        return VOS_TRUE;
    }
}



VOS_UINT8  NAS_MM_RcvMmcDetachReq_PreProc(
    VOS_VOID                           *pstRcvMsg
)
{
    MMCMM_DETACH_REQ_STRU              *pstDetachReq;

    pstDetachReq = (MMCMM_DETACH_REQ_STRU*)pstRcvMsg;

    switch (pstDetachReq->enDetachCause)
    {
        case MMC_MM_PS_DETACH:
            return MM_EVENT_ID_INVALID;
        case MMC_MM_CS_DETACH:
            if (VOS_TRUE == NAS_MM_ProcCsDetach(pstDetachReq))
            {
                return MM_EVENT_ID_INVALID;
            }
            break;
        case MMC_MM_PS_CS_DETACH:
            if (VOS_TRUE == NAS_MM_ProcCsPsDetach(pstDetachReq))
            {
                return MM_EVENT_ID_INVALID;
            }
            break;
        default:
            PS_LOG(WUEPS_PID_MM, VOS_NULL, LOG_LEVEL_WARNING,"DETACH TYPE INVALID");
            return MM_EVENT_ID_INVALID;
    }

    /* 启动CS域保护定时器  */
    NAS_MM_StartDetachProtectingTimer(pstDetachReq);

    return MM_EVENT_ID_DETACH_REQ;
}
VOS_VOID NAS_MM_RcvDetachReq_InterRatChange(
    VOS_VOID                          *pMsg
)
{
    /* 直接回复MMCMM_DETACH_CNF */
    NAS_MM_SndMmcDetachCnf(g_MmGlobalInfo.ucMmServiceState);

    /* 清除等待DETACH结果标志位 */
    g_MmGlobalInfo.stDetachInfo.enDetachType = MM_WAIT_NULL_DETACH;

}

VOS_UINT32 NAS_MM_RcvMmcLauReq(
    VOS_VOID                           *pstRcvMsg
)
{
    MMCMM_LAU_REQ_STRU                 *pstLauReq       = VOS_NULL_PTR;
    VOS_UINT8                           ucPsRestrictionFlg;

    pstLauReq               = (MMCMM_LAU_REQ_STRU*)pstRcvMsg;
    ucPsRestrictionFlg      = NAS_MML_GetPsRestrictRegisterFlg();
  
    /* NMO1下,MM不需要进行注册 */
    if ((VOS_TRUE       == NAS_MML_GetPsAttachAllowFlg())
     && (MM_NET_MODE_I  == g_MmGlobalInfo.ucNewNtMod)
     && (MM_FALSE       == g_MmGlobalInfo.ucLikeB)
     && (VOS_FALSE      == ucPsRestrictionFlg))
    {
        return MM_EVENT_ID_INVALID; 
    }    

    /* 检测是否被在当前网络上被拒#17(MMC已判断),且失败达到最大次数 */
    if ( g_MmGlobalInfo.LuInfo.ucLuAttmptCnt < MM_CONST_NUM_4 )
    {
        return MM_EVENT_ID_INVALID; 
    }

    if ( (MM_IDLE_ATTEMPTING_TO_UPDATE == Mm_GetState())
      && (MMC_MM_LAU_REQ_REASON_SOR == pstLauReq->enLauReqReason) )
    {
        /* 直接进行lau */
        g_MmGlobalInfo.LuInfo.ucLuType = MM_IE_LUT_NORMAL_LU;
        Mm_ComLuOnly();
    }       

    return MM_EVENT_ID_INVALID;
}


#if (FEATURE_ON == FEATURE_IMS)
VOS_VOID NAS_MM_RcvMmcSrvccInfoNtf(
    VOS_VOID                           *pstRcvMsg
)
{
    /* 记录SRVCC标记到MM全局变量中 */
    NAS_MM_SetSrvccFlg(VOS_TRUE);

    if (VOS_FALSE == NAS_MML_IsCsfbServiceStatusExist())
    {
        /* 通知CC模块SRVCC过程开始 */
        NAS_MM_SndCcSrvccStatusInd(NAS_MMCC_SRVCC_STATUS_START);
    }

    return;
}



VOS_VOID NAS_MM_RcvCcSrvccCallInfoNtf(
    MMCC_SRVCC_CALL_INFO_NOTIFY_STRU   *pstCcMsg
)
{
    VOS_UINT8                           i;
    VOS_UINT8                           ucTI;

#if 0
    /* 如果MM不在INTER_RAT_CHANGE状态下收到,认为时序存在问题,丢弃该消息 */
    if (MM_INTER_RAT_CHANGE != g_MmGlobalInfo.ucState)
    {
        NAS_LOG(WUEPS_PID_MM, MM_SUSPEND, PS_PRINT_ERROR,"NAS_MM_RcvCcSrvccCallInfoNtf Unexpected msg!");

        return;
    }
#endif
    
    /* 边界检查，避免数组越界 */
    if (pstCcMsg->ucTiNum > NAS_MM_MAX_CC_CONNECTION_NUM)
    {
        pstCcMsg->ucTiNum = NAS_MM_MAX_CC_CONNECTION_NUM;
    }

    /* 按照协议规定，SRVCC成功到GU后,都当做MT CALL */
    for (i = 0; i < pstCcMsg->ucTiNum ; i++)
    {
        ucTI = pstCcMsg->aucTransactionId[i];

        if (MM_CONST_NUM_7 > ucTI)
        {
            g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].aucMMConnExtFlg[0] |=
                        (VOS_UINT8 )( 0x01 << ucTI );
        }
        else if ((MM_CONST_NUM_7 < ucTI) 
              && (MM_CONST_NUM_15 > ucTI))
        {
            g_MmGlobalInfo.ConnCtrlInfo[MM_CONN_CTRL_CC].aucMMConnExtFlg[1] |=
                        (VOS_UINT8 )( 0x01 << (ucTI - 8) );
        }
        else
        {
        }
    }

    return;
}
#endif
#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif

