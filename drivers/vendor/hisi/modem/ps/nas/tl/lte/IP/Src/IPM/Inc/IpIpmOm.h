

#ifndef __IPIPMOM_H__
#define __IPIPMOM_H__

/*****************************************************************************
  1 Include Headfile
*****************************************************************************/
#include "vos.h"
#include "PsTypeDef.h"
#include "OmCommon.h"
#include "NasIpInterface.h"


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
  2 Macro
*****************************************************************************/
#define IP_PRINT_BUFF_LEN               (1024)                      /* 每次打印预设缓存 */
#define IP_PRINT_MAX_BYTES_PER_LINE     (16)                        /* 每行打印字节数   */


#define IP_GET_IP_PRINT_BUF()           (g_acIpPrintBuf)

#define IP_PRINT_MAX_ITEM_LEN           (80)                        /* 数组中字符串最大长度 */


/*========================= OSA类封装 =========================BEGIN */
#define IP_LOG_SUBMODID                 0

#define IP_LOG_LEVEL_OFF                LOG_LEVEL_OFF        /* 关闭打印     */
#define IP_LOG_LEVEL_ERROR              LOG_LEVEL_ERROR      /* Error级别    */
#define IP_LOG_LEVEL_WARNING            LOG_LEVEL_WARNING    /* Warning级别  */
#define IP_LOG_LEVEL_NORMAL             LOG_LEVEL_NORMAL     /* Normal级别   */
#define IP_LOG_LEVEL_INFO               LOG_LEVEL_INFO       /* Info级别     */


#ifdef  PS_ITT_PC_TEST
#define IP_LOG_INFO(String)             vos_printf(" %s\r\n",String)
#define IP_LOG_NORM(String)             vos_printf(" %s\r\n",String)
#define IP_LOG_WARN(String)             vos_printf(" %s\r\n",String)
#define IP_LOG_ERR(String)              vos_printf(" %s\r\n",String)

#define IP_LOG1_INFO(String, Para1)     vos_printf(" %s %d\r\n",String,Para1)
#define IP_LOG1_NORM(String, Para1)     vos_printf(" %s %d\r\n",String,Para1)
#define IP_LOG1_WARN(String, Para1)     vos_printf(" %s %d\r\n",String,Para1)
#define IP_LOG1_ERR(String,  Para1)     vos_printf(" %s %d\r\n",String,Para1)

#define IP_LOG2_INFO(String, Para1, Para2)  vos_printf(" %s %d,%d\r\n",String,Para1,Para2)
#define IP_LOG2_NORM(String, Para1, Para2)  vos_printf(" %s %d,%d\r\n",String,Para1,Para2)
#define IP_LOG2_WARN(String, Para1, Para2)  vos_printf(" %s %d,%d\r\n",String,Para1,Para2)
#define IP_LOG2_ERR(String,  Para1, Para2)  vos_printf(" %s %d,%d\r\n",String,Para1,Para2)

#define IP_LOG3_INFO(String, Para1, Para2, Para3)   vos_printf(" %s %d,%d,%d\r\n",String,Para1,Para2,Para3)
#define IP_LOG3_NORM(String, Para1, Para2, Para3)   vos_printf(" %s %d,%d,%d\r\n",String,Para1,Para2,Para3)
#define IP_LOG3_WARN(String, Para1, Para2, Para3)   vos_printf(" %s %d,%d,%d\r\n",String,Para1,Para2,Para3)
#define IP_LOG3_ERR(String,  Para1, Para2, Para3)   vos_printf(" %s %d,%d,%d\r\n",String,Para1,Para2,Para3)

#else  /*For PS_ITT_PC_TEST*/

#define IP_LOG_INFO(String)             LPS_LOG(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_INFO, String)
#define IP_LOG_NORM(String)             LPS_LOG(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_NORMAL, String)
#define IP_LOG_WARN(String)             LPS_LOG(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_WARNING, String)
#define IP_LOG_ERR(String)              LPS_LOG(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_ERROR, String)
#define IP_LOG1_INFO(String, Para1)     LPS_LOG1(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_INFO, String, (long)Para1)
#define IP_LOG1_NORM(String, Para1)     LPS_LOG1(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_NORMAL, String, (long)Para1)
#define IP_LOG1_WARN(String, Para1)     LPS_LOG1(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_WARNING, String, (long)Para1)
#define IP_LOG1_ERR(String,  Para1)     LPS_LOG1(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_ERROR, String, (long)Para1)

#define IP_LOG2_INFO(String, Para1, Para2)  LPS_LOG2(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_INFO, String, (long)Para1, (long)Para2)
#define IP_LOG2_NORM(String, Para1, Para2)  LPS_LOG2(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_NORMAL, String, (long)Para1, (long)Para2)
#define IP_LOG2_WARN(String, Para1, Para2)  LPS_LOG2(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_WARNING, String, (long)Para1, (long)Para2)
#define IP_LOG2_ERR(String,  Para1, Para2)  LPS_LOG2(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_ERROR,   String, (long)Para1, (long)Para2)

#define IP_LOG3_INFO(String, Para1, Para2, Para3)   LPS_LOG3(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_INFO, String, (long)Para1, (long)Para2, (long)Para3)
#define IP_LOG3_NORM(String, Para1, Para2, Para3)   LPS_LOG3(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_NORMAL, String, (long)Para1, (long)Para2, (long)Para3)
#define IP_LOG3_WARN(String, Para1, Para2, Para3)   LPS_LOG3(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_WARNING, String, (long)Para1, (long)Para2, (long)Para3)
#define IP_LOG3_ERR(String,  Para1, Para2, Para3)   LPS_LOG3(UE_MODULE_IP_ID, IP_LOG_SUBMODID, IP_LOG_LEVEL_ERROR,   String, (long)Para1, (long)Para2, (long)Para3)

#endif



#if 0
/* IP层内存拷贝宏定义 */
#define IP_MEM_CPY(pDestBuffer,pSrcBuffer,Count) \
    {\
        if(IP_NULL_PTR == PS_MEM_CPY_ALL_CHECK((pDestBuffer),(pSrcBuffer),(Count)))\
        {\
            IP_LOG_ERR("IP_MEM_CPY  ERR!!");\
        }\
    }

/* IP层内存置位宏定义 */
#define IP_MEM_SET(pBuffer,ucData,Count) \
    {\
        if(IP_NULL_PTR == PS_MEM_SET_ALL_CHECK( (pBuffer), (ucData), (Count)))\
        {\
            IP_LOG_ERR("IP_MEM_SET  ERR!!");\
        }\
    }

/* IP层内存比较宏定义 */
#define IP_MEM_CMP(pucDestBuffer, pucSrcBuffer, ulBufferLen)\
            VOS_MemCmp ((pucDestBuffer), (pucSrcBuffer), (ulBufferLen))

#endif
/* IP层内存分配宏定义 */
#define IP_MEM_ALLOC(ulSize) \
            PS_MEM_ALLOC_All_CHECK((PS_PID_IP), (ulSize))

/* IP层内存分配宏定义 */
#define IP_MEM_FREE(pAddr) \
    {\
        if(IP_OK != PS_MEM_FREE_ALL_CHECK((PS_PID_IP), (pAddr)))\
        {\
            IP_LOG_ERR("IP_MEM_FREE  ERR!!");\
        }\
    }

/* IP层DOPRA消息申请宏定义 */
#define IP_ALLOC_MSG(ulLen)\
            PS_ALLOC_MSG_ALL_CHECK((PS_PID_IP),(ulLen))

/* IP层内存释放宏定义 */
#define IP_FREE_MSG(pData)\
            PS_FREE_MSG_ALL_CHECK((PS_PID_IP),(pData))

#define IP_SEND_MSG(pMsg) \
        {\
            IP_PrintIpSendMsg((const PS_MSG_HEADER_STRU *)pMsg, IP_GET_IP_PRINT_BUF());\
            if(IP_OK != PS_SND_MSG_ALL_CHECK((PS_PID_IP), (pMsg)))\
            {\
                IP_LOG_ERR("IP_SEND_MSG  ERR!!");\
            }\
        }

#define IP_TTF_MEM_COPY_ALLOC(pData, usLen)     LUP_MemCopyAlloc(PS_PID_IP, pData, usLen)

#define IP_TTF_MEM_FREE(pMem)                   LUP_MemFree(PS_PID_IP, pMem)

#define IP_TTF_MEM_GET_LEN(pMem)                LUP_MemGetLen(PS_PID_IP, pMem)

#define IP_TTF_MEM_GET(pMemSrc, usOffset, pDest, usLen)\
            LUP_MemGet(PS_PID_IP, pMemSrc, usOffset, pDest, usLen)

#define IP_START_REL_TIMER(phTm, ulLength, ulName)\
            PS_START_REL_TIMER(phTm, PS_PID_IP, ulLength, ulName, ulName, VOS_RELTIMER_NOLOOP)

#define IP_STOP_REL_TIMER(phTm)         PS_STOP_REL_TIMER(phTm)

#define IP_GET_REL_TIMER_NAME(pMsg)     (((REL_TIMER_MSG *)pMsg)->ulName)

#define IP_GET_REL_TIMER_PARA(pMsg)     (((REL_TIMER_MSG *)pMsg)->ulPara)



/*****************************************************************************
  3 Massage Declare
*****************************************************************************/


/*****************************************************************************
  4 Enum
*****************************************************************************/

enum    IP_OM_AIR_MSG_DIR_ENUM
{
    IP_OMT_AIR_MSG_UP              = 0x00,             /* UPWARD */
    IP_OMT_AIR_MSG_DOWN            = 0x01,             /* DOWNWARD */

    IP_OMT_AIR_MSG_BUTT
};
typedef VOS_UINT8   IP_OM_AIR_MSG_DIR_ENUM_UINT8;

enum    IP_OM_AIR_MSG_ID_ENUM
{
    IP_DHCPV4_DISCOVER,
    IP_DHCPV4_OFFER,
    IP_DHCPV4_REQUEST,
    IP_DHCPV4_ACK,
    IP_DHCPV4_NAK,
    IP_DHCPV4_INFORM
};
typedef VOS_UINT8   IP_OM_AIR_MSG_ID_ENUM_UINT8;



enum    IP_OMT_KEY_EVENT_TYPE_ENUM
{
    IP_OMT_KE_DHCPV4_RELEASE,

    IP_OMT_KE_TYPE_BUTT
};
typedef VOS_UINT8   IP_OMT_KEY_EVENT_TYPE_ENUM_UINT8;

/*****************************************************************************
  5 STRUCT
*****************************************************************************/
/*****************************************************************************
 结构名    : IP_PRINT_ACTION_FUN
 协议表格  :
 ASN.1描述 :
 结构说明  : 打印动作函数类型的定义
*****************************************************************************/
typedef VOS_INT32 (* IP_PRINT_ACTION_FUN)
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    const PS_MSG_HEADER_STRU           *pstMsg
);

/*****************************************************************************
 结构名    : IP_PRINT_MSG_LIST_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 打印消息结构体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulId;
    VOS_UINT8                           aucPrintString[IP_PRINT_MAX_ITEM_LEN];
    IP_PRINT_ACTION_FUN                 pfActionFun;
}IP_PRINT_MSG_LIST_STRU;


/*****************************************************************************
  6 UNION
*****************************************************************************/


/*****************************************************************************
  7 Extern Global Variable
*****************************************************************************/
extern VOS_CHAR                         g_acIpPrintBuf[IP_PRINT_BUFF_LEN];

/*****************************************************************************
  8 Fuction Extern
*****************************************************************************/
extern VOS_VOID    IP_Nsprintf
(
   VOS_CHAR                           *pcBuff,
   VOS_UINT16                          usOffset,
   const VOS_CHAR                     *String,
   VOS_INT32                          *pilOutLen
);
extern VOS_VOID    IP_Nsprintf_1
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    const VOS_CHAR                     *String,
    VOS_UINT32                          para1,
    VOS_INT32                          *pilOutLen
);
extern VOS_VOID    IP_Nsprintf_2
(
   VOS_CHAR                           *pcBuff,
   VOS_UINT16                          usOffset,
   const VOS_CHAR                     *String,
   VOS_UINT32                          para1,
   VOS_UINT32                          para2,
   VOS_INT32                          *pilOutLen
);
extern VOS_VOID    IP_Print
(
   VOS_CHAR                           *pcBuff,
   VOS_UINT16                          usLen
);
extern VOS_VOID   IP_PrintArray
(
    VOS_CHAR                           *pcBuff,
    const VOS_UINT8                    *pucArray,
    VOS_UINT32                          ulLen
);
extern VOS_INT32   IP_PrintData
(
   VOS_CHAR                           *pcBuff,
   VOS_UINT16                          usOffset,
   NAS_IP_MSG_STRU                    *pstMsg
);
extern VOS_INT32  IP_PrintEsmIpConfigParaIndMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    const PS_MSG_HEADER_STRU           *pstMsg
);
extern VOS_INT32  IP_PrintEsmIpConfigParaReqMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    const PS_MSG_HEADER_STRU           *pstMsg
);
extern VOS_INT32  IP_PrintEsmIpMsg
(
   VOS_CHAR                            *pcBuff,
   VOS_UINT16                           usOffset,
   const PS_MSG_HEADER_STRU            *pstMsg
);
extern VOS_INT32  IP_PrintEsmIpPdpReleaseIndMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    const PS_MSG_HEADER_STRU           *pstMsg
);
extern VOS_INT32  IP_PrintIpRabmDataIndMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    const PS_MSG_HEADER_STRU           *pstMsg
);
extern VOS_INT32  IP_PrintIpRabmStartOrStopFilterReqMsg
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    const PS_MSG_HEADER_STRU           *pstMsg
);
extern VOS_VOID IP_PrintIpRevMsg
(
   const PS_MSG_HEADER_STRU           *pstMsg,
   VOS_CHAR                           *pcBuff
);
extern VOS_VOID IP_PrintIpSendMsg
(
   const PS_MSG_HEADER_STRU           *pstMsg,
   VOS_CHAR                           *pcBuff
);
extern VOS_INT32  IP_PrintIpTimer
(
    VOS_CHAR                           *pcBuff,
    VOS_UINT16                          usOffset,
    VOS_UINT32                          ulTimerName
);
extern VOS_INT32  IP_PrintRabmIpMsg
(
   VOS_CHAR                            *pcBuff,
   VOS_UINT16                           usOffset,
   const PS_MSG_HEADER_STRU            *pstMsg
);

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

#endif /* end of IpIpmOm.h */

