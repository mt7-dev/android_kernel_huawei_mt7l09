

#ifndef TC_EXT_H_
#define TC_EXT_H_

#include "Tcresourceinfo.h"


#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif /* __cpluscplus */
#endif /* __cpluscplus */


extern TC_GLOBAL_CTRL_STRU               g_TcInfo;                              /* TC全局控制信息           */

VOS_VOID Tc_TaskInit();

VOS_VOID Tc_RcvMmDataInd(VOS_VOID *pMsg);
VOS_VOID Tc_RcvGmmDataInd(VOS_VOID *pMsg);
VOS_VOID Tc_RcvRabmTestCnf(VOS_VOID *pMsg);
VOS_VOID Tc_RcvRabmTestLoopCnf();
VOS_VOID Tc_RcvRabmRabInd(VOS_VOID *pMsg);
VOS_VOID Tc_SndRlcTcModeInfo(VOS_UINT8 ucTcFlag, VOS_UINT8 ucTcMode);

VOS_VOID Tc_RcvCloseLoop(VOS_UINT8 * pRcvTcMsg);
VOS_VOID Tc_RcvOpenLoop();
VOS_VOID Tc_RcvActRbTest();
VOS_VOID Tc_RcvDeactRbTest();

VOS_VOID Tc_SndMtaResetStoredPosInfoInd(TC_UE_POS_TECH_ENUM_UINT8 enUePosTech);
VOS_VOID Tc_RcvResetUePosStoredInfo(VOS_UINT8 * pRcvTcMsg);

VOS_VOID Tc_SndDataReq(VOS_UINT8 ucMsgType);
VOS_VOID Tc_SndMmTestReq(VOS_UINT8 ucMode);
VOS_VOID Tc_SndGmmTestReq(VOS_UINT8 ucMode);
VOS_VOID Tc_SndSmTestReq(VOS_UINT8 ucMode);
VOS_VOID Tc_SndSmsTestReq(VOS_UINT8 ucMode);
VOS_VOID Tc_SndSsTestReq(VOS_UINT8 ucMode);
VOS_VOID Tc_SndCcTestReq(VOS_UINT8 ucMode);
VOS_VOID Tc_SndRabmTestReq(VOS_UINT8 ucMode);
VOS_VOID Tc_SndRabmTestLoopReq(VOS_UINT8 ucMode);

VOS_UINT32  Tc_GetState(VOS_VOID);

VOS_UINT32  Tc_GetMode(VOS_VOID);

VOS_UINT32  Tc_GetMsgSize( VOS_UINT16 usMsgId );
VOS_INT32   Tc_SndPhyMsg( VOS_UINT16 usMsgId, VOS_UCHAR *pucStru );
VOS_VOID    Tc_SndOpenLoopAck(VOS_UINT8 ucAckInfo);
VOS_UINT16  Tc_ParseTchLoopType( VOS_UINT8 ucSubChannel );
VOS_UINT16  Tc_ParseTchChannelType( VOS_UINT8 ucSubChannel );
VOS_UINT16  Tc_ParseTestDevice( VOS_UINT8 ucTestDeviceIE );
VOS_VOID    Tc_RcvPhyCloseTchLoopCnf(TC_CLOSE_TCH_LOOP_CNF_STRU *pCloseTchLoopCnf);
VOS_VOID    Tc_RcvPhyOpenTchLoopCnf(TC_OPEN_TCH_LOOP_CNF_STRU *pOpenTchLoopCnf);
VOS_VOID    Tc_RcvPhyTestInterfaceCnf(TC_TEST_INTERFACE_CNF_STRU *pTestInterfaceCnf);
VOS_VOID    Tc_RcvCloseTchLoopCmd(VOS_UCHAR *pRcvTcMsg);
VOS_VOID    Tc_RcvOpenLoopCmd(VOS_UCHAR *pRcvTcMsg);
VOS_VOID    Tc_RcvTestInterface(VOS_UCHAR *pRcvTcMsg);
VOS_VOID    Tc_RcvHpaMsgHandle(struct MsgCB *pMsg);

VOS_UINT32  Tc_SndGrmGtmSetupReq(VOS_UCHAR *pMsg);
VOS_VOID    Tc_SndGrmGtmRelRsp(VOS_VOID);
VOS_VOID    Tc_RcvGrmGtmSetupCnf(VOS_UCHAR *pMsg);
VOS_VOID    Tc_RcvGrmGtmRelInd(VOS_VOID);
VOS_VOID    Tc_RcvGrmMsgHandle(struct MsgCB *pMsg);
VOS_VOID    Tc_RcvGprsTestModeCmd(VOS_UCHAR *pRcvTcMsg);

VOS_UINT32  Tc_SndGrmGtmEgprsSetupReq(VOS_UINT8 ucModeFlg);
VOS_VOID    Tc_SndGrmGtmEgprsRelRsp(VOS_VOID);
VOS_VOID    Tc_RcvGrmGtmEgprsSetupCnf(VOS_UCHAR *pMsg);
VOS_VOID    Tc_RcvGrmGtmEgprsRelInd(VOS_VOID);
VOS_VOID    Tc_RcvEgprsLoopBackCmd(VOS_UINT8 ucModeFlg);

/*TC环回闪灯:调用该接口来模拟MMC发给MMA一条PDP ACTIVE/DEACTIVE的消息*/
VOS_VOID NAS_MMC_SndMmaDataTranAttri(
    VOS_UINT8                           ucDataTranAttri
);

VOS_VOID NAS_TC_RcvGmmRelInd(
    struct MsgCB                       *pstMsg
);

VOS_VOID NAS_TC_RcvMmRelInd(
    struct MsgCB                       *pstMsg
);

VOS_VOID NAS_TC_RcvGmmHandoverInd(
    struct MsgCB                       *pstMsg
);
VOS_VOID Tc_SndRfaTestControlMsg(VOS_UINT32 ulMsgName);
VOS_VOID NAS_TC_SndWphyCloseLoopNtf(VOS_VOID);
VOS_VOID NAS_TC_SndWphyOpenLoopNtf(VOS_VOID);

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif/* __cpluscplus */

#endif  /* TC_EXT_H_ */

