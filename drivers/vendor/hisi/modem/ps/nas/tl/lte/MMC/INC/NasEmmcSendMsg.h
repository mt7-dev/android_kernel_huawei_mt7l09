

#ifndef __NASEMMCSENDMSG_H__
#define __NASEMMCSENDMSG_H__

/*****************************************************************************
1 Include Headfile
*****************************************************************************/
#include  "NasEmmcPublic.h"



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



/*****************************************************************************
3 Massage Declare
*****************************************************************************/


/*****************************************************************************
4 Enum
*****************************************************************************/



/*****************************************************************************
5 STRUCT
*****************************************************************************/





/*****************************************************************************
6 UNION
*****************************************************************************/


/*****************************************************************************
7 Extern Global Variable
*****************************************************************************/
extern VOS_UINT32  NAS_LMM_MmcPlmnReqTypeCheck
(
    MMC_LMM_PLMN_SRCH_TYPE_ENUM_UINT32 ulMmcPlmnType
);
extern VOS_UINT32  NAS_LMM_RrcPlmnCnfTypeCheck
(
    LRRC_LNAS_PLMN_SEARCH_RLT_ENUM_UINT32   ulPlmnRslt
);
extern VOS_VOID  NAS_EMMC_SendEmmStartCnf(VOS_VOID);
extern VOS_VOID  NAS_EMMC_SendEmmStopCnf(VOS_VOID);
extern VOS_VOID  NAS_EMMC_SendEmmPlmnInd(VOS_VOID);

extern VOS_VOID  NAS_EMMC_SendRrcPlmnSrchReq(const MMC_LMM_PLMN_SRCH_REQ_STRU *pstRcvMsg);


extern VOS_VOID  NAS_EMMC_SendMmcPlmnSrchCnf(const LRRC_LMM_PLMN_SEARCH_CNF_STRU *pstRcvMsg );
extern VOS_VOID  NAS_EMMC_SendMmcPlmnListSrchRej(VOS_VOID );
/* lihong00150010 emergency tau&service begin */
extern VOS_VOID  NAS_EMMC_SendEmmSysInfoInd
(
    EMMC_EMM_CHANGE_INFO_ENUM_UINT32 ulChangeInfo,
    EMMC_EMM_CELL_STATUS_ENUM_UINT32 ulCellStatus,
    EMMC_EMM_FORBIDDEN_INFO_ENUM_UINT32  ulForbdInfo
);
/* lihong00150010 emergency tau&service end */
extern VOS_VOID  NAS_EMMC_SendMmcSysInfo(const LRRC_LMM_SYS_INFO_IND_STRU  *pstRcvMsg  );
extern VOS_VOID  NAS_EMMC_SendRrcEplmnNotifyReq(const MMC_LMM_EPLMN_NOTIFY_REQ_STRU *pstRcvMsg);
extern VOS_VOID  NAS_EMMC_SendRrcPlmnSrchStopReq( VOS_VOID);
extern VOS_VOID  NAS_EMMC_SendMmcStopSrchCnf(VOS_VOID );

extern VOS_VOID  NAS_EMMC_SendMmcAreaLostInd(VOS_VOID  );

extern VOS_VOID NAS_EMMC_SendRrcBgPlmnSearchReq(const MMC_LMM_BG_PLMN_SEARCH_REQ_STRU *pstMmcMsg);
extern VOS_VOID NAS_EMMC_SendRrcBgPlmnSearchStopReq(const MMC_LMM_STOP_BG_PLMN_SEARCH_REQ_STRU *pstMmcMsg);
extern VOS_VOID NAS_EMMC_SendMmcBgPlmnSearchCnf(const LRRC_LMM_BG_PLMN_SEARCH_CNF_STRU *pstLrrcMsg);
extern VOS_VOID NAS_EMMC_SendMmcBgPlmnSearchStopCnf(const LRRC_LMM_BG_PLMN_SEARCH_STOP_CNF_STRU *pstLrrcMsg);
extern VOS_VOID NAS_EMMC_SendMmcNotCampOnInd(VOS_VOID);

extern VOS_VOID  NAS_EMMC_SendMmcSearchPlmnInfoInd
(
    const LRRC_LMM_SEARCHED_PLMN_INFO_IND_STRU *pstLrrcMsg
);


/*****************************************************************************
8 Fuction Extern
*****************************************************************************/


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

#endif  /* end of NasEmmcSendMsg.h */



