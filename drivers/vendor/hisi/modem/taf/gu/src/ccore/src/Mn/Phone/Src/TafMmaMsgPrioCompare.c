

/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include  "TafMmaMsgPrioCompare.h"
#include  "TafFsm.h"
#include  "Taf_Tafm_Remote.h"
#include  "TafOamInterface.h"
#include  "TafLog.h"
#include  "TafMmaSndInternalMsg.h"

#include  "MmaMmcInterface.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_TAF_MMA_MSG_PRIO_COMPARE_C

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/

TAF_MMA_MSG_COMPARE_STRU g_astMmaMsgPrioCompareWithPhoneModeTbl[] =
{
    /* phone mode过程中收到OM的模式设置请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_OM, OAM_MMA_PHONE_MODE_SET_REQ),
                             TAF_MMA_ComparePhoneModeSetReqPrioWithPhoneMode),

    /* 模式查询不需要比较 */



    /* phone mode过程中收到模式设置请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_PHONE_MODE_SET_REQ),
                             TAF_MMA_ComparePhoneModeSetReqPrioWithPhoneMode),

    /* phone mode过程中收到MMA的内部卡状态指示的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_MMA, MMA_MMA_INTER_USIM_STATUS_CHANGE_IND),
                             TAF_MMA_CompareMmaInterUsimStatusChangeIndPrioWithPhoneMode),


    /* phone mode过程中收到TAF的SYSCFG设置请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_SYS_CFG_SET_REQ),
                             TAF_MMA_CompareAtSyscfgReqPrioWithPhoneMode),

    /* phone mode过程中收到AT的list搜网请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_AT, TAF_MSG_MMA_PLMN_LIST),
                             TAF_MMA_CompareAtPlmnListReqPrioWithPhoneMode),

    /* phone mode过程中收到AT的用户指定搜网请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_AT, TAF_MSG_MMA_PLMN_USER_SEL),
                             TAF_MMA_CompareAtPlmnUserSelReqPrioWithPhoneMode),

    /* phone mode过程中收到AT的attach请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_AT, TAF_MSG_MMA_ATTACH),
                             TAF_MMA_CompareAtAttachPrioWithPhoneMode),


    /* phone mode过程中收到AT的detach请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_DETACH_REQ),
                             TAF_MMA_CompareAtDetachPrioWithPhoneMode),

    /* phone mode过程中收到AT的自动重选请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_AT, TAF_MSG_MMA_PLMN_RESEL),
                             TAF_MMA_CompareAtPlmnReselPrioWithPhoneMode),

#if (FEATURE_ON == FEATURE_IMS)
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_IMS_SWITCH_SET_REQ),
                             TAF_MMA_CompareImsSwitchSetReqPrioWithPhoneMode),
#endif

};

#if (FEATURE_ON == FEATURE_IMS)

TAF_MMA_MSG_COMPARE_STRU g_astMmaMsgPrioCompareWithImsSwitchTbl[] =
{
    /* IMS SWITCH过程中收到OM的模式设置请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_OM, OAM_MMA_PHONE_MODE_SET_REQ),
                             TAF_MMA_ComparePhoneModeSetReqPrioWithImsSwitch),

    /* IMS SWITCH过程中收到模式设置请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_PHONE_MODE_SET_REQ),
                             TAF_MMA_ComparePhoneModeSetReqPrioWithImsSwitch),

    /* IMS SWITCH过程中收到MMA的内部卡状态指示的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_MMA, MMA_MMA_INTER_USIM_STATUS_CHANGE_IND),
                             TAF_MMA_CompareMmaInterUsimStatusChangeIndPrioWithImsSwitch),

    /* IMS SWITCH过程中收到TAF的SYSCFG设置请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_SYS_CFG_SET_REQ),
                             TAF_MMA_CompareAtSyscfgReqPrioWithImsSwitch),

    /* IMS SWITCH过程中收到AT的list搜网请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_AT, TAF_MSG_MMA_PLMN_LIST),
                             TAF_MMA_CompareAtPlmnListReqPrioWithImsSwitch),

    /* IMS SWITCH过程中收到AT的用户指定搜网请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_AT, TAF_MSG_MMA_PLMN_USER_SEL),
                             TAF_MMA_CompareAtPlmnUserSelReqPrioWithImsSwitch),

    /* IMS SWITCH过程中收到AT的attach请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_AT, TAF_MSG_MMA_ATTACH),
                             TAF_MMA_CompareAtAttachPrioWithImsSwitch),

    /* IMS SWITCH过程中收到AT的detach请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_DETACH_REQ),
                             TAF_MMA_CompareAtDetachPrioWithImsSwitch),

    /* IMS SWITCH过程中收到AT的自动重选请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_AT, TAF_MSG_MMA_PLMN_RESEL),
                             TAF_MMA_CompareAtPlmnReselPrioWithImsSwitch),

    /* IMS SWITCH过程中收到TAF的IMS开关机请求的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_TAF, ID_TAF_MMA_IMS_SWITCH_SET_REQ),
                             TAF_MMA_CompareImsSwitchSetReqPrioWithImsSwitch),


    /* IMS SWITCH过程中收到MMC的SYSCFG回复的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_MMC, ID_MMC_MMA_SYS_CFG_CNF),
                             TAF_MMA_CompareMmcSyscfgCnfPrioWithImsSwitch),

    /* IMS SWITCH过程中收到MMC的PLMN RESEL回复的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_MMC, ID_MMC_MMA_PLMN_RESEL_CNF),
                             TAF_MMA_CompareMmcPlmnReselCnfPrioWithImsSwitch),

    /* IMS SWITCH过程中收到MMC的PLMN RESEL回复的消息优先级比较 */
    TAF_MMA_COMPARE_TBL_ITEM(TAF_BuildEventType(WUEPS_PID_MMC, ID_MMC_MMA_DETACH_CNF),
                             TAF_MMA_CompareMmcDetachCnfPrioWithImsSwitch),

};
#endif
TAF_MMA_FSM_MSG_COMPARE_STRU g_astMmaMsgCompareTbl[] =
{
     /* phone mode 状态机比较函数表*/
    TAF_MMA_FSM_COMPARE_TBL_ITEM(TAF_MMA_FSM_PHONE_MODE,
                                 g_astMmaMsgPrioCompareWithPhoneModeTbl),

#if (FEATURE_ON == FEATURE_IMS)
    /* IMS SWITCH状态机比较函数表*/
    TAF_MMA_FSM_COMPARE_TBL_ITEM(TAF_MMA_FSM_IMS_SWITCH,
                                 g_astMmaMsgPrioCompareWithImsSwitchTbl),
#endif
};

/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_UINT32 TAF_MMA_ComparePhoneModeSetReqPrioWithPhoneMode(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}



VOS_UINT32 TAF_MMA_CompareAtSyscfgReqPrioWithPhoneMode(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}



VOS_UINT32 TAF_MMA_CompareMmaInterUsimStatusChangeIndPrioWithPhoneMode(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtPlmnListReqPrioWithPhoneMode(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtPlmnUserSelReqPrioWithPhoneMode(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtAttachPrioWithPhoneMode(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}



VOS_UINT32 TAF_MMA_CompareAtDetachPrioWithPhoneMode(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtPlmnReselPrioWithPhoneMode(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}

#if (FEATURE_ON == FEATURE_IMS)

VOS_UINT32 TAF_MMA_CompareImsSwitchSetReqPrioWithPhoneMode(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_ComparePhoneModeSetReqPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtSyscfgReqPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareMmaInterUsimStatusChangeIndPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtPlmnListReqPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtPlmnUserSelReqPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtAttachPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtDetachPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareAtPlmnReselPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareImsSwitchSetReqPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareMmcSyscfgCnfPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareMmcPlmnReselCnfPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}


VOS_UINT32 TAF_MMA_CompareMmcDetachCnfPrioWithImsSwitch(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    *penAbortType   = TAF_MMA_ABORT_BUTT;

    return TAF_MMA_MSG_COMPARE_PRIO_RSLT_STORE;
}
#endif



TAF_MMA_MSG_COMPARE_FUNC  TAF_MMA_FindMsgPrioCompareFun(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_FSM_ID_ENUM_UINT32          enFsmId
)
{
    TAF_MMA_FSM_MSG_COMPARE_STRU       *pstFsmCompareTbl = VOS_NULL_PTR;
    VOS_UINT32                          i;
    VOS_UINT32                          ulTblSize;

    ulTblSize = sizeof(g_astMmaMsgCompareTbl)/sizeof(TAF_MMA_FSM_MSG_COMPARE_STRU);

    for ( i = 0 ; i < ulTblSize ; i++ )
    {
        if ( enFsmId == g_astMmaMsgCompareTbl[i].enFsmId)
        {
            pstFsmCompareTbl = &g_astMmaMsgCompareTbl[i];
            break;
        }
    }

    /* 未能找到该状态机的比较函数表,直接返回空指针 */
    if ( VOS_NULL_PTR == pstFsmCompareTbl )
    {
        return VOS_NULL_PTR;
    }

    for ( i = 0 ; i < pstFsmCompareTbl->ulSize; i++ )
    {
        /* 查找不同事件的比较函数 */
        if ( pstFsmCompareTbl->pfCompareTbl[i].ulCurrEventType == ulEventType)
        {
            return pstFsmCompareTbl->pfCompareTbl[i].pfCompareFun;
        }
    }

    /* 未找到处理函数直接返回空指针 */
    return VOS_NULL_PTR;
}
TAF_MMA_MSG_COMPARE_PRIO_RSLT_ENUM_UINT32 TAF_MMA_GetMsgComparePrioRslt(
    VOS_UINT32                          ulEventType,
    struct MsgCB                       *pstMsg,
    TAF_MMA_ABORT_FSM_TYPE_UINT8       *penAbortType
)
{
    TAF_MMA_MSG_COMPARE_FUNC                    pfCompareFunc;
    TAF_MMA_FSM_ID_ENUM_UINT32                  enCurrFsmId;
    TAF_MMA_MSG_COMPARE_PRIO_RSLT_ENUM_UINT32   enRslt;

    /*  获取当前状态机的ID */
    enCurrFsmId         = TAF_MMA_GetCurrFsmId();

    if (TAF_MMA_FSM_BUTT <= enCurrFsmId)
    {
        TAF_ERROR_LOG(WUEPS_PID_MMA, "ERROR:FsmId Error");
    }

    /*  获取比较函数 TAF_MMA_FindMsgPrioCompareFun() */
    pfCompareFunc       = TAF_MMA_FindMsgPrioCompareFun(ulEventType, pstMsg, enCurrFsmId);

    /* 未找到处理函数,则默认消息丢弃 */
    if ( VOS_NULL_PTR == pfCompareFunc )
    {
        *penAbortType = TAF_MMA_ABORT_BUTT;
        return TAF_MMA_MSG_COMPARE_PRIO_RSLT_DISCARD;
    }

    /* 调用比较函数,返回优先级和打断类型 */
    enRslt = pfCompareFunc(ulEventType, pstMsg, penAbortType);

    return enRslt;

}




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

