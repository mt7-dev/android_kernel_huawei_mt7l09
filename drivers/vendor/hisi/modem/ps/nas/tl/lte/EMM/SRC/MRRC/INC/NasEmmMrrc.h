

#ifndef __NASEMMMRRC_H__
#define __NASEMMMRRC_H__



/*****************************************************************************
  1 Include Headfile
*****************************************************************************/

/*****************************************************************************
  1.1 Cplusplus Announce
*****************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  #pragma pack(*)    设置字节对齐方式
*****************************************************************************/
#if (VOS_OS_VER != VOS_WIN32)
#pragma pack(4)
#else
#pragma pack(push, 4)
#endif
/*****************************************************************************
  2 macro
*****************************************************************************/
/*lint -e961*/
#define NAS_LMM_RRC_OPID_NOT_VAILID                (0xFFFFFFFF)
/*lint +e961*/

#define NAS_LMM_MRRC_NAS_MSG_MAX_LEN                   1024

#define NAS_LMM_RRC_SAVE_MSG_MAX_NUM                6
#define NAS_EMM_GET_MRRC_MGMT_DATA_ADDR()         (&(g_stEmmMrrcMgmtData[0]))

#if 0
#define NAS_EMM_GET_MRRC_SEND_STATE()             (gstEmmMrrcMgmtData.enMrrcSendState)
#define NAS_EMM_CHANGE_MRRC_SEND_STATE(STATE)     (gstEmmMrrcMgmtData.enMrrcSendState = STATE)
#endif

#define NAS_EMM_INIT_MM_RRC_OPID()                (gulMmRrcOpId = 0)
/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
#if 0
/*****************************************************************************
 ???ù??    : NAS_EMM_MRRC_SEND_STATE_ENUM
 D-òé±í??  :
 ASN.1?èê? :
 ???ù?μ?÷  : MRRC′?ê?×′ì?
*****************************************************************************/
enum NAS_EMM_MRRC_SEND_STATE_ENUM
{
    NAS_EMM_MRRC_SEND_STATE_READY       = 0,
    NAS_EMM_MRRC_SEND_STATE_PENDING        ,
    NAS_EMM_MRRC_SEND_STATE_BUTT
};
typedef VOS_UINT32 NAS_EMM_MRRC_SEND_STATE_ENUM_UINT32;
#endif
/*****************************************************************************
  5 STRUCT
*****************************************************************************/
/*****************************************************************************
 结构名    : NAS_EMM_MRRC_MGMT_DATA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : NAS_EMM_MRRC_MGMT_DATA_STRU数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulEsmMmOpId;       /* 记录Esm和EMM间的OPID*/
    VOS_UINT32                          ulMmRrcOpId;       /* 记录EMM和RRC间的OPID*/
    NAS_EMM_MSG_TYPE_ENUM_UINT32        enLastMsgId;       /* 记录EMM给RRC发送的消息ID*/

    #if 0
    NAS_EMM_MRRC_SEND_STATE_ENUM_UINT32 enMrrcSendState;    /* ????MRRCμ?·￠?í×′ì?*/
    #endif
    VOS_UINT32                          ulHoWaitSysInfoFlag;
    VOS_UINT32                          ulNasMsgLength;     /* ??????RRC·￠?í???￠μ?3¤?è*/
    VOS_UINT8                           aucNasMsgContent[NAS_LMM_MRRC_NAS_MSG_MAX_LEN]; /*??????RRC·￠?í???￠μ??úèY */
}NAS_EMM_MRRC_MGMT_DATA_STRU;

/* ?ˉ×÷′|àíoˉêyμ?ààDí?¨ò? */
typedef VOS_UINT32 ( * NAS_LMM_SEND_MSG_RESULT_FAIL_ACTION_FUN )
(
    VOS_VOID *  /* ???￠???? */,VOS_UINT32 *
);

typedef VOS_UINT32 ( * NAS_LMM_SEND_MSG_RESULT_SUCC_ACTION_FUN )
(
    VOS_VOID *  /* ???￠???? */
);


/* ?ˉ×÷±í?á11 */
typedef struct
{
    NAS_EMM_MSG_TYPE_ENUM_UINT32               enMsgId;            /* ???￠ID*/
    NAS_LMM_SEND_MSG_RESULT_FAIL_ACTION_FUN         pfFailActionFun;    /* ê§°ü?ˉ×÷oˉêy */
    NAS_LMM_SEND_MSG_RESULT_SUCC_ACTION_FUN         pfSuccActionFun;    /* 3é1|?ˉ×÷oˉêy */
}NAS_LMM_SEND_MSG_RESULT_ACT_STRU;
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
extern NAS_EMM_MRRC_MGMT_DATA_STRU      g_stEmmMrrcMgmtData[NAS_LMM_RRC_SAVE_MSG_MAX_NUM];

extern NAS_LMM_SEND_MSG_RESULT_ACT_STRU  gstEmmMrrcSendMsgResultActTbl[17];
extern VOS_UINT32        g_ulSendMsgResultActTblLen;

extern VOS_UINT32              g_ulSaveMsgNum;


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_VOID    NAS_EMM_IntraEstReq(const NAS_EMM_MRRC_DATA_REQ_STRU *pMsg);
extern VOS_VOID    NAS_EMM_RrcEstReq(const NAS_EMM_MRRC_DATA_REQ_STRU *pMsg);
extern VOS_VOID    NAS_EMM_RrcDataReq(const NAS_EMM_MRRC_DATA_REQ_STRU *pMsg,VOS_UINT32 ulMmRrcOpId);
extern VOS_VOID    NAS_EMM_MrrcConnectFailInd(LRRC_LNAS_EST_RESULT_ENUM_UINT32 enEstResult);
extern VOS_VOID    NAS_EMM_SndRrcRelReq(NAS_LMM_BARRED_IND_ENUM_UINT32 enBarStatus);

extern VOS_VOID    NAS_EMM_MrrcEstInit(VOS_VOID);
extern VOS_VOID    NAS_EMM_MrrcRelInit(VOS_VOID);

extern VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgDrbSetupInd(VOS_UINT32 ulMsgId, const VOS_VOID *pMsgStru);
extern VOS_UINT32  NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsg3440Exp(VOS_UINT32 ulMsgId, const VOS_VOID *pMsgStru);
extern VOS_UINT32 NAS_EMM_MsRrcConnRelInitSsWaitRrcRelMsgReattachDelayTmrExp(VOS_UINT32 ulMsgId, const VOS_VOID *pMsgStru);

extern VOS_VOID    NAS_EMM_MrrcMgmtDataInit( VOS_VOID );
extern VOS_UINT32  NAS_EMM_MrrcAllocMmRrcOpId( VOS_VOID );
/*extern VOS_UINT32  NAS_EMM_ChkMmRrcOpId( VOS_UINT32 ulOpId );*/
/*extern VOS_VOID    NAS_EMM_ClrMrrcMgmtData( NAS_EMM_STATE_TI_ENUM_UINT16 enTmr );*/

extern VOS_VOID  NAS_EMM_SaveMrrcDataReqMsg
(
    const NAS_EMM_MRRC_DATA_REQ_STRU        *pstMsg,
    VOS_UINT32                              ulMmRrcOpId
);
extern NAS_EMM_MRRC_MGMT_DATA_STRU* NAS_EMM_FindMsgInDataReqBuffer(VOS_UINT32 ulOpId);

extern VOS_VOID  NAS_EMM_ClrAllUlDataReqBufferMsg( VOS_VOID );

extern VOS_VOID  NAS_EMM_ClrAllUlDataReqBufferMsgExceptMtDetachAcc( VOS_VOID );

extern VOS_VOID  NAS_EMM_ClrDataReqBufferMsg
(
    VOS_UINT32                          ulMmRrcOpId
);
extern VOS_VOID  NAS_EMM_SaveCurMrrcDataReqMsg( NAS_EMM_MRRC_MGMT_DATA_STRU    *pEmmMrrcMgmtData );
extern VOS_UINT32  NAS_EMM_ClrHoWaitSysInfoBufferMsg( VOS_VOID );

extern VOS_UINT32  NAS_EMM_MsRrcConnEstInitSsWaitRrcConnMsgSysinfo( VOS_UINT32  ulMsgId,
                                                             VOS_VOID   *pMsgStru );
extern VOS_UINT32  NAS_EMM_MsRrcConnEstInitSsWaitRrcConnMsgT3412Exp( VOS_UINT32  ulMsgId,
                                                              VOS_VOID   *pMsgStru  );
extern VOS_UINT32  NAS_EMM_MsRrcConnEstInitWaitRrcConnMsgEsmDataReq (VOS_UINT32  ulMsgId,
                                                              VOS_VOID   *pMsgStru  );
extern VOS_VOID    NAS_EMM_FillEstInfo(
                                LRRC_LMM_EST_REQ_STRU              *pstEstReqMsg,
                                NAS_EMM_MSG_TYPE_ENUM_UINT32        enEmmMsgType );
/*leili modify for isr begin*/
extern VOS_VOID  NAS_EMM_CcoInterSystemSuccProc( VOS_VOID );          
extern VOS_VOID  NAS_EMM_CcoInterSystemFailProc( VOS_VOID );
/*leili modify for isr end*/
/*****************************************************************************
  9 OTHERS
*****************************************************************************/


#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack(pop)
#endif


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of 发送模块+接收模块+Interface.h */
