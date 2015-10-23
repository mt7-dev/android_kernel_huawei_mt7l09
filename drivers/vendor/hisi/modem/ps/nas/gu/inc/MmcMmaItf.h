

#ifndef _MMC_MMA_Itf_H_
#define _MMC_MMA_Itf_H_

#ifdef  __cplusplus
  #if  __cplusplus
  extern "C"{
  #endif
#endif


/*****************************************************************************
   1 头文件包含
*****************************************************************************/
#include "vos.h"
#include "wuepscfg.h"
#include "asnasapi.h"
#include "GmmDef.h"
#include "GmmStru.h"
#include "MmcMmInterface.h"

/* 删除ExtAppMmcInterface.h*/

#include "TafClientApi.h"
#include "NasMmlCtx.h"
#include "TafAppMma.h"

#pragma pack(4)
/*****************************************************************************
   2 全局变量定义
*****************************************************************************/

/*定义RSSI和RSCP的无效值，MMC和MMA共用*/
#define MMC_MMA_UTRA_RSCP_UNVALID            (-140)
#define MMC_MMA_UTRA_RSSI_UNVALID            (-120)




typedef enum
{
    EN_MMC_PLMN_LIST_INFO,           /*获取完整的PLMN列表*/
    EN_MMC_OPLMN_INFO,
    EN_MMC_UPLMN_INFO,
    EN_MMC_HPLMN_INFO,
    EN_MMC_PLMNSEL_INFO,

    /*设置MMC全局变量参数*/
    EN_MMC_BEGIN_SET_MMC_PARA,
    EN_MMC_SET_PLMNSEL_INFO,
    EN_MMC_SET_OPLMN_INFO,
    EN_MMC_SET_UPLMN_INFO,
    EN_MMC_END_SET_MMC_PARA,
    MMC_MMA_PARA_TYPE_BUTT
}MMC_MMA_PARA_TYPE_ENUM;
typedef struct
{
    MMA_MMC_PLMN_ID_STRU               stPlmn;
    VOS_UINT16                         usRaMode;
    VOS_UINT8                          ucStatus;

    VOS_UINT8               aucReserve1[1];
}MMC_TAF_PLMN_INFO_ST;

typedef struct
{
    MMC_TAF_PLMN_INFO_ST    astPlmnInfo[TAF_USER_MAX_PLMN_NUM];
    VOS_UINT16              usCurPlmnNum;

    VOS_UINT8               aucReserve1[2];
}MMC_TAF_PLMN_LIST_INFO_ST;


typedef struct
{
  MMC_MMA_PARA_TYPE_ENUM    enMmaParaType;
  union
  {
    MMC_TAF_PLMN_LIST_INFO_ST           stHPlmn;            /*获取HPLMN列表*/
    MMC_TAF_PLMN_LIST_INFO_ST           stUPlmnInfo;
    MMC_TAF_PLMN_LIST_INFO_ST           stOPlmnInfo;
    MMC_TAF_PLMN_LIST_INFO_ST           stHPlmnInfo;
  }u;
}MMC_MMA_SHARE_PARA_ST;



/*****************************************************************************
   3 函数原型
*****************************************************************************/
extern VOS_UINT32 Api_MmaGetParaFromMmc(MMC_MMA_SHARE_PARA_ST *pstMmcPara);
extern VOS_UINT32 Api_MmaSetParaFromMmc(const MMC_MMA_SHARE_PARA_ST *pstMmcPara);


/* 函数Mma_TimeOutProc删除 */

extern VOS_VOID Api_MmaFplmnHandleFromMmc(VOS_UINT16        ClientId,
                                          VOS_UINT8               OpId,
                                          TAF_PH_FPLMN_OPERATE_STRU stFplmnOperate);
extern VOS_VOID MMC_FplmnAdd(VOS_UINT16        ClientId,
                             VOS_UINT8               OpId,
                            TAF_PLMN_ID_STRU     *pstPlmn);

extern VOS_VOID MMC_FplmnDel(
    VOS_UINT16                          ClientId,
    VOS_UINT8                           OpId,
    TAF_PH_FPLMN_OPERATE_STRU          *pstFplmnOperate
);
extern VOS_VOID MMC_FplmnQurey(VOS_UINT16        ClientId,
                                   VOS_UINT8               OpId);

extern VOS_BOOL NAS_MMA_GetServiceDomainSet(VOS_VOID);

#if ((VOS_OS_VER == VOS_WIN32) || (VOS_OS_VER == VOS_NUCLEUS))
#pragma pack()
#else
#pragma pack(0)
#endif
#ifdef  __cplusplus
  #if  __cplusplus
  }
  #endif
#endif



#endif /* end of MMA_MMC_Itf.h*/

