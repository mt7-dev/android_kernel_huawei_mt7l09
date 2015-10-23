

#ifndef __LRABMPDCPINTERFACE_H__
#define __LRABMPDCPINTERFACE_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "LUPZeroCopy.h"

#pragma pack(4)

/*****************************************************************************
  2 macro
*****************************************************************************/
#define LPS_SECU_IV_BYTE_NUM     16
#define LPS_SECU_KEY_BYTE_NUM    16

/*****************************************************************************
  3 Message Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
enum PS_SECU_ALG_ENUM
{
    PS_SECU_ALG_NULL            = 0,
    PS_SECU_ALG_SNOW_3G         = 1,
    PS_SECU_ALG_AES             = 2,

    PS_SECU_ALG_BUTT
};
typedef VOS_UINT8   PS_SECU_ALG_ENUM_UINT8;

/*****************************************************************************
 结构名    : PS_SECU_OP_ENUM
 协议表格  :
 ASN.1描述 :
 结构说明  :
*****************************************************************************/
enum PS_SECU_OP_ENUM
{
    PS_SECU_OP_CIPHER            = 0,
    PS_SECU_OP_UNCIPHER          = 1,
    PS_SECU_OP_PROTECT           = 2,
    PS_SECU_OP_VERIFY            = 3,

    PS_SECU_OP_BUTT
};
typedef VOS_UINT8   PS_SECU_OP_ENUM_UINT8;


/*****************************************************************************
   5 STRUCT
*****************************************************************************/
/*****************************************************************************
 结构名    : LPDCP_RABM_DATA_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP与RABM用户面数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRbId;
    VOS_UINT8                           aucRev[3];
    VOS_UINT32                          ulSduLen;
    LUP_MEM_ST                         *pstSdu;
}LPDCP_RABM_DATA_STRU;

/*****************************************************************************
 结构名    : LPDCP_RABM_IND_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : PDCP与RABM用户面数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucRbId;
    VOS_UINT8                           aucRev[3];
    VOS_UINT32                          ulSduLen;
    TTF_MEM_ST                         *pstSdu;
}LPDCP_RABM_IND_STRU;


typedef struct
{
    PS_SECU_ALG_ENUM_UINT8      enAlgType;
    PS_SECU_OP_ENUM_UINT8       enOpType;
    VOS_UINT8                   aucRev[2];
    VOS_UINT8                   aucKey[LPS_SECU_KEY_BYTE_NUM];
    VOS_UINT8                   aucIV[LPS_SECU_IV_BYTE_NUM];
    VOS_UINT8                  *pucSrc;
    VOS_UINT32                  ulSrcLen;
    VOS_UINT8                  *pucDest;
}PS_SECU_PARA_STRU;

/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/


/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
#if 0
extern VOS_VOID  LPDCP_DataReq (LPDCP_RABM_DATA_STRU *pstData);
#endif
extern VOS_UINT32 LPS_NasDataSecuProc(PS_SECU_PARA_STRU const * pstSecuPara);

extern VOS_VOID LPS_NasSecuEngineInit(VOS_VOID);


/*****************************************************************************
  9 OTHERS
*****************************************************************************/









#if (VOS_OS_VER != VOS_WIN32)
#pragma pack()
#else
#pragma pack()
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of LRabmPdcpInterface.h */
