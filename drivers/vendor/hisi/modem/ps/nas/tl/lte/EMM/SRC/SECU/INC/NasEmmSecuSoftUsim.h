

#ifndef __NASEMMSECUSOFTUSIM_H__
#define __NASEMMSECUSOFTUSIM_H__



/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "PsCommonDef.h"

#include "NasEmmPubUGlobal.h"

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
#define NAS_EMM_USIM_RAND_LEN        16
#define NAS_EMM_USIM_AUTN_LEN        16

#if 0
/*#define NAS_EMM_USIM_SQN_LEN         6*/
#define NAS_EMM_USIM_AMF_LEN         2
#define NAS_EMM_USIM_MAC_LEN         8
#define NAS_EMM_USIM_NAS_KEY_LEN     16
#define NAS_EMM_MAX_USIM_RES_LEN     16
#define NAS_EMM_SOFT_USIM_RES_LEN    8
#define NAS_EMM_USIM_AUTS_LEN        14
#define NAS_EMM_USIM_AK_LEN          6

#define NAS_EMM_LEN_MSG_HEADER       20

#define NAS_EMM_L_BYTE_OF_TLV        1

#define NAS_EMM_USIM_MAC_POS         8

#define NAS_EMM_MAX_AUTH_CNF_MSG_LEN 100

#define NAS_EMM_SOFT_USIM_NOSUPPORT_SQN         0
#define NAS_EMM_SOFT_USIM_SUPPORT_SQN           1

/* SQN verify */
/*#define NAS_EMM_USIM_SEQ_ARRAY_LEN   32*/
/*#define NAS_EMM_USIM_SEQ_UINT32_LEN  2*/
#define NAS_EMM_USIM_MASK_IND        0x1F
#define NAS_EMM_USIM_MASK_SEQ_LOWEST 0xE0

#define NAS_EMM_USIM_SEQ_LOW_PART    0
#define NAS_EMM_USIM_SEQ_HIGH_PART   1

#define NAS_EMM_USIM_AMF_RESYNC           0xff

#define NAS_EMM_USIM_GET_SEQ_FROM_SQN(stSeq,Sqn)\
    {\
        stSeq.aulSEQ[0] = (VOS_UINT32)((Sqn[0] << NAS_EMM_MOVEMENT_16_BITS)\
                                        |(Sqn[1] << NAS_EMM_MOVEMENT_8_BITS)\
                                        |(Sqn[2]& NAS_EMM_USIM_MASK_SEQ_LOWEST))\
                                        >> 5;\
        stSeq.aulSEQ[1] = (VOS_UINT32)(((Sqn[2]& NAS_EMM_USIM_MASK_IND)\
                                        << NAS_EMM_MOVEMENT_24_BITS)\
                                        |(Sqn[3] << NAS_EMM_MOVEMENT_16_BITS)\
                                        |(Sqn[4] << NAS_EMM_MOVEMENT_8_BITS)\
                                        |Sqn[5]) >> 5;\
    }

#define NAS_EMM_USIM_GET_IND_FROM_SQN(Ind,Sqn)\
    {\
        Ind = Sqn[5] & NAS_EMM_USIM_MASK_IND;\
    }

/* 内存拷贝宏定义 */
#define EMM_USIM_MEM_CPY(pDestBuffer,pSrcBuffer,Count) \
            PS_MEM_CPY( (pDestBuffer), (pSrcBuffer), (Count))

#define EMM_USIM_MEM_SET(pBuffer,ucData,Count) \
            PS_MEM_SET( (pBuffer), (ucData), (Count))

#define EMM_USIM_MEM_CMP(pData1,pData2,Size) \
            PS_MEM_CMP( (pData1), (pData2), (Size))

#define NAS_EMM_GetSoftUsimAuthParamAddr()    (&g_stSoftUsimAuthParam)

#define NAS_EMM_GetSoftUsimAuthAlg() (NAS_EMM_GetSoftUsimAuthParamAddr()->enSoftUsimAlg)
#define NAS_EMM_GetSoftUsimKeyAddr() (NAS_EMM_GetSoftUsimAuthParamAddr()->aucKey)
#define NAS_EMM_GetSoftUsimOpAddr()  (NAS_EMM_GetSoftUsimAuthParamAddr()->aucOp)

#endif

/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/
/* USIM AUTH RSLT */
enum    NAS_EMM_USIM_AUTH_RSLT_ENUM
{

    NAS_EMM_USIM_AUTH_MAC_FAILURE                   = 0x14,     /* USIM返回MAC不匹配错误 */
    NAS_EMM_USIM_AUTH_SYNCH_FAILURE                 = 0x15,     /* USIM返回SYNCH不匹配错误 */
    NAS_EMM_USIM_AUTH_SUCC,
    NAS_EMM_USIM_AUTH_RELT_BUTT
};
typedef VOS_UINT8 NAS_EMM_USIM_AUTH_RSLT_ENUM_UINT8;
#if 0
enum    NAS_EMM_USIM_DERIVE_SQN_RESULT_ENUM
{

    NAS_EMM_USIM_DERIVE_SQN_FAILURE,                    /* USIM生成SQN失败 */
    NAS_EMM_USIM_DERIVE_SQN_SUCC,                       /* USIM生成SQN成功 */

    NAS_EMM_USIM_DERIVE_SQN_RESULT_BUTT
};
typedef VOS_UINT32 NAS_EMM_USIM_DERIVE_SQN_RESULT_ENUM_UINT32;

/*****************************************************************************
 枚举名    : NAS_EMM_MEMORY_COMPARE_RESULT_ENUM
 结构说明  : 内存比较结果
*****************************************************************************/
enum NAS_EMM_USIM_MEMORY_COMPARE_RESULT_ENUM
{
   NAS_EMM_USIM_MEMORY_COMPARE_EQUAL                     = 0,    /* 相等 */
   NAS_EMM_USIM_MEMORY_COMPARE_NOT_EQUAL                 = 1,    /* 不相等*/

   NAS_EMM_USIM_MEMORY_COMPARE_RESULT_BUTT
};
typedef VOS_UINT8   NAS_EMM_USIM_MEMORY_COMPARE_RESULT_ENUM_UINT8;

/*****************************************************************************
   5 STRUCT
*****************************************************************************/

/*****************************************************************************
结构名称    :NAS_EMM_USIM_RES_STRU
使用说明    :鉴权过程中RES的数据结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucResLen;
    VOS_UINT8                           aucRsv[3];
    VOS_UINT8                           aucRes[NAS_EMM_MAX_USIM_RES_LEN];
}NAS_EMM_USIM_RES_STRU;
/*****************************************************************************
结构名称    :NAS_EMM_USIM_PARAM_STRU
使用说明    :NAS鉴权参数
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucRand[NAS_EMM_USIM_RAND_LEN];
    VOS_UINT8                           aucAutn[NAS_EMM_USIM_AUTN_LEN];
}NAS_EMM_USIM_PARAM_STRU;

/*****************************************************************************
结构名称    :NAS_EMM_USIM_DERIVE_RESULT_STRU
使用说明    :NAS鉴权参数
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           aucCK[NAS_EMM_USIM_NAS_KEY_LEN];
    VOS_UINT8                           aucIK[NAS_EMM_USIM_NAS_KEY_LEN];
    NAS_EMM_USIM_RES_STRU               stRes;
    VOS_UINT8                           aucAuts[NAS_EMM_USIM_AUTS_LEN];
    VOS_UINT8                           aucRsv[2];
}NAS_EMM_USIM_DERIVE_RESULT_STRU;
#endif
/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
#if 0
extern NAS_EMM_USIM_CONTROL_STRU            g_stUsimInfo;
extern NAS_EMM_SOFT_USIM_AUTH_PARAM_STRU    g_stSoftUsimAuthParam;
#endif
/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/

extern VOS_VOID PS_GetLeastSignBitsByTrunc(const VOS_UINT8 aucSrcData[MAX_HMAC_HASH_SIZE],
                                              VOS_UINT8 aucDestData[MAX_HMAC_HASH_SIZE/2]);
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

#endif /* end of NasEmmSecuSoftUsim.h */
