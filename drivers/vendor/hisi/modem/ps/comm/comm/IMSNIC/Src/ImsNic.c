/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ImsNic.c
  版 本 号   : 初稿
  作    者   : caikai
  生成日期   : 2013年7月22日
  最近修改   :
  功能描述   : VoLTE特性中IMS虚拟网卡的处理
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月22日
    作    者   : caikai
    修改内容   : 创建文件

******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif


/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "ImsNic.h"

#if( FEATURE_ON == FEATURE_IMS )
#include "DrvInterface.h"
#include <stdio.h>
#include "stddef.h"

#if (VOS_OS_VER == VOS_WIN32)
#include  "VxWorksStub.h"
#else
#include  "v_sock.h"
#include  "muxLib.h"
#include  "endLib.h"
#endif


/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_IMS_NIC_C

LOCAL STATUS IMS_NIC_Start(IMS_NIC_DEV_STRU *pstImsNicDev);
LOCAL STATUS IMS_NIC_Stop(IMS_NIC_DEV_STRU *pstImsNicDev);
LOCAL STATUS IMS_NIC_Unload(IMS_NIC_DEV_STRU *pstImsNicDev);
LOCAL VOS_INT IMS_NIC_Ioctl(IMS_NIC_DEV_STRU *pstImsNicDev, VOS_INT cmd, caddr_t data);
LOCAL STATUS IMS_NIC_Send(IMS_NIC_DEV_STRU *pstImsNicDev, M_BLK_ID pstBuf);
LOCAL STATUS IMS_NIC_MCastAddrAdd(IMS_NIC_DEV_STRU *pstImsNicDev, VOS_CHAR *pAddress);
LOCAL STATUS IMS_NIC_MCastAddrDel(IMS_NIC_DEV_STRU *pstImsNicDev, VOS_CHAR *pAddress);
LOCAL STATUS IMS_NIC_MCastAddrGet(IMS_NIC_DEV_STRU *pstImsNicDev, MULTI_TABLE *pstMultiTable);
LOCAL STATUS IMS_NIC_PollSend(IMS_NIC_DEV_STRU *pstImsNicDev, M_BLK_ID pstBuf);
LOCAL STATUS IMS_NIC_PollRcv(IMS_NIC_DEV_STRU *pstImsNicDev, M_BLK_ID pstBuf);
#if 0
LOCAL M_BLK_ID IMS_NIC_FormAddress(
    M_BLK_ID                            pNBuff,
    M_BLK_ID                            pSrcAddr,
    M_BLK_ID                            pDstAddr,
    BOOL                                bcastFlag
);
LOCAL STATUS IMS_NIC_PacketDataGet(M_BLK_ID pMblk, LL_HDR_INFO *pLinkHdrInfo);
LOCAL STATUS IMS_NIC_AddrGet(
    M_BLK_ID                  pMblk,
    M_BLK_ID                  pSrc,
    M_BLK_ID                  pDst,
    M_BLK_ID                  pESrc,
    M_BLK_ID                  pEDst
);
#endif

#ifdef  ST_IMS_NIC
extern VOS_VOID  ST_IMS_NIC_InitQ(VOS_VOID);
extern VOS_UINT32  ST_IMS_NIC_PrintData( VOS_UINT8 *pData, VOS_UINT16 usLen );
#endif

/*****************************************************************************
  2 全局变量定义
*****************************************************************************/
IMS_NIC_ENTITY_STRU                     g_astImsNicEntity[IMS_NIC_MODEM_ID_BUTT] = {{0}};  /* IMS虚拟网卡实体信息 */

const IMS_NIC_MANAGE_TBL_STRU           g_stImsNicManageTbl    =
{
    {
        (FUNCPTR) IMS_NIC_Start,                                                     /* Function to start the device. */
        (FUNCPTR) IMS_NIC_Stop,                                                      /* Function to stop the device. */
        (FUNCPTR) IMS_NIC_Unload,                                                    /* Unloading function for the driver. */
        (FUNCPTR) IMS_NIC_Ioctl,                                                     /* Ioctl function for the driver. */
        (FUNCPTR) IMS_NIC_Send,                                                      /* Send function for the driver. */
        (FUNCPTR) IMS_NIC_MCastAddrAdd,                                              /* Multicast add function for the driver. */
        (FUNCPTR) IMS_NIC_MCastAddrDel,                                              /* Multicast delete function for the driver. */
        (FUNCPTR) IMS_NIC_MCastAddrGet,                                              /* Multicast retrieve function for the driver. */
        (FUNCPTR) IMS_NIC_PollSend,                                                  /* Polling send function */
        (FUNCPTR) IMS_NIC_PollRcv,                                                   /* Polling receive function */
        endEtherAddressForm,                                                         /* put address info into a NET_BUFFER */
        endEtherPacketDataGet,                                                       /* get pointer to data in NET_BUFFER */
        endEtherPacketAddrGet                                                        /* Get packet addresses. */
    },
    "ims_nic",
    {
        {
            {
                {
                    0x58, 0x02, 0x03, 0x04, 0x05, 0x06
                },
                {
                    0x00, 0x11, 0x09, 0x64, 0x01, 0x01
                },
                0x0008
            },
            {
                {
                    0x58, 0x02, 0x03, 0x04, 0x05, 0x06
                },
                {
                    0x00, 0x11, 0x09, 0x64, 0x01, 0x01
                },
                0xDD86
            }
        }
    },
    {
        "ipdnsc.primaryns",
        "ipdnsc.secondaryns",
        "ipdnsc.tertiaryns",
        "ipdnsc.quaternaryns"
    }
};

IMS_NIC_CTX_STRU    g_stImsNicCtx   =
{
    IMS_NIC_MTU_DEFAULT_VALUE,
    0,
    0,
    {
        VOS_NULL_PTR
    }
};

IMS_NIC_DEV_STRU                    g_astImsNicDev[IMS_NIC_MODEM_ID_BUTT];                  /* 虚拟网卡设备空间 */

/* 调试时使用的打印开关 */
VOS_UINT8       g_ucImsNicDebugPrint    = 0;

/* 调试打印 */
#define IMS_NIC_DEBUG_PRINT(...)\
        {\
            if ( 1 ==  g_ucImsNicDebugPrint)\
            {\
                vos_printf(__VA_ARGS__);\
            }\
        }

#define         IMS_NIC_OK              (0)
#define         IMS_NIC_ERR             (1)
#define         IMS_NIC_ERROR           (-1)
#define         IMS_NIC_INVAL           (EINVAL)    /* 入参无效 */
#define         IMS_NIC_NOTSUP          (ENOTSUP)   /* 不支持 */


/*****************************************************************************
  3 函数实现
*****************************************************************************/

VOS_VOID IMS_NIC_MNTN_InitLogCnt(IMS_NIC_LOG_ENT_STRU *pstLogEnt)
{
    if(VOS_OK != VOS_SmP(g_stImsNicCtx.ulInitLogMutexSem, 0))
    {
        return;
    }

    pstLogEnt->ulCnt = 0;

    VOS_SmV(g_stImsNicCtx.ulInitLogMutexSem);

    return;
}


VOS_VOID IMS_NIC_MNTN_LogOutput(IMS_NIC_LOG_ENT_STRU *pstLogEnt, VOS_UINT32 ulPid)
{
    IMS_NIC_TRACE_LOG_MSG_STRU          stImsTraceLogMsg;


    /*  避免递归死循环 */
    if( (pstLogEnt->ulCnt > IMS_NIC_LOG_RECORD_MAX_NUM) || (0 == pstLogEnt->ulCnt) )
    {
        IMS_NIC_MNTN_InitLogCnt(pstLogEnt);

        return;
    }

    stImsTraceLogMsg.ulSenderCpuId     = VOS_LOCAL_CPUID;
    stImsTraceLogMsg.ulSenderPid       = ulPid;
    stImsTraceLogMsg.ulReceiverCpuId   = VOS_LOCAL_CPUID;
    stImsTraceLogMsg.ulReceiverPid     = ulPid;
    stImsTraceLogMsg.ulLength          = (sizeof(IMS_NIC_TRACE_LOG_MSG_STRU) - VOS_MSG_HEAD_LENGTH);

    stImsTraceLogMsg.usMsgType         = ID_IMS_NIC_MNTN_TRACE_LOG_MSG;
    stImsTraceLogMsg.usTransId         = 0;

    DRV_RT_MEMCPY( &stImsTraceLogMsg.stLogMsgCont, pstLogEnt, sizeof(IMS_NIC_LOG_ENT_STRU) );

    OM_TraceMsgHook(&stImsTraceLogMsg);

    IMS_NIC_MNTN_InitLogCnt(pstLogEnt);

    return ;
}
VOS_VOID IMS_NIC_MNTN_LogSave
(
    IMS_NIC_LOG_ENT_STRU           *pstLogEnt,
    VOS_UINT32                      ulPid,
    VOS_UINT32                      enLevel,
    PS_FILE_ID_DEFINE_ENUM_UINT32   enFile,
    VOS_UINT32                      ulLine,
    VOS_INT32                       lpara1,
    VOS_INT32                       lpara2,
    VOS_INT32                       lpara3,
    VOS_INT32                       lpara4
)
{
    VOS_UINT32      ulCnt;
    VOS_UINT32      ulCurrentSlice;
    VOS_UINT32      ulTimeStamp;
    VOS_UINT32      ulTimeStampTmp;


    /* 为了与SDT里面的TimeStamp一致，根据SDT的换算方法:(0xFFFFFFFF - OM_GetSlice())%32768*100，对Slice进行换算 */
    ulCurrentSlice  = OM_GetSlice();
    /* Slice为递减时，ulCurrentSlice  = 0xFFFFFFFF - ulCurrentSlice，递增时，不用减 */

    ulTimeStampTmp  = ulCurrentSlice & 0x7FFF;
    ulTimeStampTmp  = ulTimeStampTmp * 100;
    ulTimeStampTmp  = ulTimeStampTmp >> 15;

    ulTimeStamp     = ulCurrentSlice >> 15;
    ulTimeStamp     = ulTimeStamp * 100;
    ulTimeStamp     = ulTimeStamp + ulTimeStampTmp;

    if(VOS_OK != VOS_SmP(g_stImsNicCtx.ulSaveLogMutexSem, 0))
    {
        return;
    }

    ulCnt           = pstLogEnt->ulCnt;

    if( ulCnt < IMS_NIC_LOG_RECORD_MAX_NUM )
    {
        pstLogEnt->astData[ulCnt].ulTick        = ulTimeStamp;
        pstLogEnt->astData[ulCnt].enFile        = enFile;
        pstLogEnt->astData[ulCnt].ulLine        = ulLine;
        pstLogEnt->astData[ulCnt].enLevel       = enLevel;
        pstLogEnt->astData[ulCnt].alPara[0]     = lpara1;
        pstLogEnt->astData[ulCnt].alPara[1]     = lpara2;
        pstLogEnt->astData[ulCnt].alPara[2]     = lpara3;
        pstLogEnt->astData[ulCnt].alPara[3]     = lpara4;
        ulCnt++;
        pstLogEnt->ulCnt                        = ulCnt;
    }

    VOS_SmV(g_stImsNicCtx.ulSaveLogMutexSem);

    /* 记录满了，自动发送 */
    if( ulCnt >= IMS_NIC_LOG_RECORD_MAX_NUM )
    {
        IMS_NIC_MNTN_LogOutput(pstLogEnt, ulPid);
    }

    return;
}
VOS_UINT32 IMS_NIC_MNTN_LogEntInit(VOS_UINT32 ulPid)
{
    VOS_UINT32      ulResult;


    PS_MEM_SET(&g_stImsNicCtx.stLogEnt, 0, sizeof(IMS_NIC_LOG_ENT_STRU));

    ulResult    = VOS_SmMCreate("INIT", VOS_SEMA4_FIFO,
                    (VOS_UINT32 *)(&g_stImsNicCtx.ulInitLogMutexSem));

    if ( VOS_OK != ulResult )
    {
        IMS_NIC_INIT_LOG_PRINT1(ulPid, "IMS_NIC_MNTN_CreateMutexSem: Create g_ulInitLogMutexSem fail!", ulResult);

        return VOS_ERR;
    }

    ulResult    = VOS_SmMCreate("SAVE", VOS_SEMA4_FIFO,
                    (VOS_UINT32 *)(&g_stImsNicCtx.ulSaveLogMutexSem));

    if ( VOS_OK != ulResult )
    {
        IMS_NIC_INIT_LOG_PRINT1(ulPid, "IMS_NIC_MNTN_CreateMutexSem: Create g_ulLogSaveMutexSem fail!", ulResult);

        return VOS_ERR;
    }

    return VOS_OK;
}


VOS_VOID IMS_NIC_MntnTrace(VOS_UINT16 usMsgId, VOS_UINT8 *pucData, VOS_UINT32 ulLen, VOS_UINT32 ulPara1, VOS_UINT32 ulPara2)
{
    IMS_NIC_MNTN_TRACE_MSG_STRU *pstMntnTrace;
    VOS_UINT32                   ulMsgLen;

    ulMsgLen = offsetof(IMS_NIC_MNTN_TRACE_MSG_STRU, aucData) + ulLen;

    pstMntnTrace = (IMS_NIC_MNTN_TRACE_MSG_STRU *)PS_MEM_ALLOC(UEPS_PID_IMSNIC, ulMsgLen);

    if (VOS_NULL_PTR == pstMntnTrace)
    {
        IMS_NIC_WARNING_LOG(UEPS_PID_IMSNIC, "IMS_NIC_MntnTrace, WARNING, Alloc msg fail.");

        return;
    }

    pstMntnTrace->ulReceiverPid        = UEPS_PID_IMSNIC;
    pstMntnTrace->ulSenderPid          = UEPS_PID_IMSNIC;
    pstMntnTrace->ulSenderCpuId        = VOS_LOCAL_CPUID;
    pstMntnTrace->ulReceiverCpuId      = VOS_LOCAL_CPUID;
    pstMntnTrace->ulLength             = ulMsgLen - VOS_MSG_HEAD_LENGTH;
    pstMntnTrace->usMsgId              = usMsgId;
    pstMntnTrace->ulPara1              = ulPara1;
    pstMntnTrace->ulPara2              = ulPara2;

    if (VOS_NULL_PTR != pucData)
    {
        DRV_RT_MEMCPY(pstMntnTrace->aucData, pucData, ulLen);
    }

    OM_TraceMsgHook(pstMntnTrace);

    PS_MEM_FREE(UEPS_PID_IMSNIC, pstMntnTrace);
}


VOS_VOID IMS_NIC_ResetAll(MODEM_ID_ENUM_UINT16 enModemId)
{
    IMS_NIC_ENTITY_STRU                 *pstImsNicEntity;
    VOS_UINT8                            aucCmd[IMS_NIC_CMD_LEN] = {0};
    VOS_INT                              iLen;


    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(enModemId);

#if 0
    /* 清IPV4路由 */
    iLen = IMS_NIC_IPV4_ROUTE_DEL_CMD(aucCmd, enModemId, IMS_NIC_GET_NIC_NAME);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    /* 清IPV6路由 */
    iLen = IMS_NIC_IPV6_ROUTE_DEL_CMD(aucCmd, enModemId, IMS_NIC_GET_NIC_NAME);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);
#endif

    /* down网卡 */
    iLen = IMS_NIC_DOWN_NIC_CMD(aucCmd, enModemId, IMS_NIC_GET_NIC_NAME);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    PS_MEM_SET(&(pstImsNicEntity->stImsNicPdnInfo), 0, sizeof(IMS_NIC_PDN_INFO_STRU));
}

/*****************************************************************************
 函 数 名  : IMS_NIC_I2A
 功能描述  : 根据转换基数(10或16), 将整数转换为ASCII码, 将结果输出至字符串
 输入参数  : usValue    - 待转换为ASCII码的整数
             pcStr      - 输出结果的字符串
             usRadix    - 转换基数
 输出参数  : 无
 返 回 值  : VOS_CHAR*
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数
*****************************************************************************/
VOS_CHAR* IMS_NIC_I2A(VOS_UINT16 usValue, VOS_CHAR *pcStr, VOS_UINT16 usRadix)
{
    if (IMS_NIC_TYPE_HEX == usRadix)
    {
        pcStr += VOS_sprintf(pcStr, "%x", usValue);
    }
    else if(IMS_NIC_TYPE_DEC == usRadix)
    {
        pcStr += VOS_sprintf(pcStr, "%d", usValue);
    }
    else
    {
        IMS_NIC_WARNING_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_I2A, WARNING, usRadix is Invalid", usRadix);
    }

    return pcStr;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_FindIpv6AddrZeroFieldsToBeCompressed
 功能描述  : 找出需要使用"::"表示的IP地址段的起始位置
 输入参数  : ausAddrValue[]     - IPV6地址段数组
             ucTokensNum        - 地址段个数
 输出参数  : pucZeroFieldStart  - 数值连续为0的IP地址段起始位置
             pucZeroFieldCount  - 数值连续为0的IP地址段个数
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID IMS_NIC_FindIpv6AddrZeroFieldsToBeCompressed(
    VOS_UINT8                          *pucZeroFieldStart,
    VOS_UINT8                          *pucZeroFieldCount,
    VOS_UINT16                         *pusAddrValue,
    VOS_UINT8                           ucTokensNum
)
{
    VOS_UINT32                          ulIndx;
    VOS_UINT8                           ucStart;
    VOS_UINT8                           ucCount;


    ucStart                             = 0xFF;
    ucCount                             = 0;

    for (ulIndx = 0; ulIndx < (VOS_UINT32)(ucTokensNum - 1); ulIndx++)
    {
        if ( (0x0000 == pusAddrValue[ulIndx])
          && (0x0000 == pusAddrValue[ulIndx + 1]) )
        {
            /* 记录数值连续为0的IP地址段起始位置 */
            if (0xFF == ucStart)
            {
                ucStart = (VOS_UINT8)ulIndx;
            }

            /* 更新数值连续为0的IP地址段个数 */
            ucCount++;
        }
        else
        {
            /* 更新待压缩的IP地址段位置, 以及IP地址段个数 */
            if (0xFF != ucStart)
            {
                if (ucCount > *pucZeroFieldCount)
                {
                    *pucZeroFieldStart = ucStart;
                    *pucZeroFieldCount = ucCount;
                }

                ucStart = 0xFF;
                ucCount = 0;
            }
        }
    }

    /* 数值连续为0的IP地址段在结尾时, 需要更新一次待压缩的IP地址段位置,
       以及IP地址段个数 */
    if (0xFF != ucStart)
    {
        if (ucCount > *pucZeroFieldCount)
        {
            *pucZeroFieldStart = ucStart;
            *pucZeroFieldCount = ucCount;
        }
    }
}


/*****************************************************************************
 函 数 名  : IMS_NIC_ConvertIpv6AddrToCompressedStr
 功能描述  : 将IPV6地址格式转换为字符串压缩格式
 输入参数  : aucIpAddr[]    - IPV6地址(协议格式)
             ucTokensNum    - 地址段个数
 输出参数  : aucAddrStr[]   - IPV6地址(字符串格式)
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID IMS_NIC_ConvertIpv6AddrToCompressedStr(
    VOS_UINT8                          *pucAddrStr,
    VOS_UINT8                          *pucIpAddr,
    VOS_UINT8                           ucTokensNum
)
{
    VOS_UINT8                          *pucBuffer;
    VOS_UINT32                          ulIndx;
    VOS_UINT16                          ausAddrValue[IMS_NIC_IPV6_STR_RFC2373_TOKENS]; /* IMS_NIC_IPV6_STR_RFC2373_TOKENS]; */
    VOS_UINT16                          usAddrNum;
    VOS_UINT8                           ucDelimiter;
    VOS_UINT8                           ucRadix;
    VOS_UINT8                           ucZeroFieldStart;
    VOS_UINT8                           ucZeroFieldCount;


    PS_MEM_SET(ausAddrValue, 0x00, sizeof(ausAddrValue));

    pucBuffer                           = pucAddrStr;
    ucDelimiter                         = IMS_NIC_IPV6_STR_DELIMITER;
    ucRadix                             = IMS_NIC_TYPE_HEX;
    ucZeroFieldStart                    = 0xFF;
    ucZeroFieldCount                    = 0;

    /* 根据IP字符串格式表达类型, 获取分段的IP地址数值 */
    for (ulIndx = 0; ulIndx < ucTokensNum; ulIndx++)
    {
        usAddrNum = *pucIpAddr++;

        usAddrNum <<= 8;
        usAddrNum  |= *pucIpAddr++;

        ausAddrValue[ulIndx] = usAddrNum;
    }

    /* 找出需要使用"::"表示的IP地址段的起始位置  */
    IMS_NIC_FindIpv6AddrZeroFieldsToBeCompressed(&ucZeroFieldStart, &ucZeroFieldCount, ausAddrValue, ucTokensNum);

    /* 遍历IP地址分段, 创建有分隔符标记的IP地址字符串 */
    for (ulIndx = 0; ulIndx < ucTokensNum; ulIndx++)
    {
        if (ucZeroFieldStart == ulIndx)
        {
            *pucBuffer++ = ucDelimiter;

            ulIndx += ucZeroFieldCount;

            /* 如果已到IP地址分段的最后一段, 需要补充分隔符 */
            if ((ucTokensNum - 1) == ulIndx)
            {
                *pucBuffer++ = ucDelimiter;
            }
        }
        else
        {
            /* 如果是IP地址分段的第一段, 不需要补充分隔符 */
            if (0 != ulIndx)
            {
                *pucBuffer++ = ucDelimiter;
            }

            pucBuffer    = (VOS_UINT8 *)IMS_NIC_I2A(ausAddrValue[ulIndx], (VOS_CHAR*)pucBuffer, ucRadix);
        }
    }

    /* 补充字符串结束符 */
    if (pucAddrStr != pucBuffer)
    {
        *pucBuffer = '\0';
    }
}

/*****************************************************************************
 函 数 名  : IMS_NIC_IsSameIpAddr
 功能描述  : 根据IPv4,IPv6地址对比两个地址是否相同
 输入参数  : pucIpAddr1            -- 需要比较的地址1
             pucIpAddr2            -- 需要比较的地址2
             ucIpAddrLen           -- IP地址长度
 输出参数  : 无
 返 回 值  : 相等返回1，不相等返回0
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_IsSameIpAddr(VOS_UINT8 *pucIpAddr1, VOS_UINT8 *pucIpAddr2, VOS_UINT8 ucIpAddrLen)
{
    VOS_UINT32 *pulIpAddrTmp1 = (VOS_UINT32 *)pucIpAddr1;
    VOS_UINT32 *pulIpAddrTmp2 = (VOS_UINT32 *)pucIpAddr2;


    if ((VOS_NULL_PTR == pucIpAddr1) || (VOS_NULL_PTR == pucIpAddr2))
    {
        IMS_NIC_ERROR_LOG2(UEPS_PID_IMSNIC, "IMS_NIC_IsSameIpAddr, ERROR, Input para is NULL.", pucIpAddr1, pucIpAddr2);

        return 0;
    }

    if (IMS_NIC_IPV4_ADDR_LENGTH == ucIpAddrLen)
    {
        return (*(pulIpAddrTmp1) == *(pulIpAddrTmp2));
    }
    else
    {
        return !((pulIpAddrTmp1[0]^pulIpAddrTmp2[0]) | (pulIpAddrTmp1[1]^pulIpAddrTmp2[1])
        | (pulIpAddrTmp1[2]^pulIpAddrTmp2[2]) | (pulIpAddrTmp1[3]^pulIpAddrTmp2[3]));
    }
}

/*****************************************************************************
 函 数 名  : IMS_NIC_DnsAddrCheck
 功能描述  : 检查待配置的DNS服务器地址是否已经配置过
 输入参数  : pucIpAddr              -- 欲配置的DNS服务器IP
             ulIpAddrLen            -- IP地址长度
             pstImsNicDnsInfo       -- 内部存储的DNS配置信息
 输出参数  : pucDnsIndx             -- 配置过则返回DNS服务器的索引，没配置过则返回可配置的DNS服务器的索引
 返 回 值  : 配置过为VOS_TRUE，没有配置过为VOS_FALSE
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_DnsAddrCheck (VOS_UINT8 *pucIpAddr, VOS_UINT8 ucIpAddrLen, IMS_NIC_DNS_INFO_STRU *pstDnsInfo, VOS_UINT8 *pucDnsIndx)
{
    VOS_UINT8                           ucIndex;
    VOS_UINT8                           aucIdle[IMS_NIC_MAX_DNS_SERVER_NUM] = {0};
    VOS_UINT8                           ucIdleNum;


    ucIdleNum   = 0;

    if ((VOS_NULL_PTR == pucIpAddr) || (VOS_NULL_PTR == pucDnsIndx) || (VOS_NULL_PTR == pstDnsInfo))
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DnsAddrCheck, ERROR, Input para is NULL.");

        return IMS_NIC_DNS_NOT_CFGED;
    }

    /* 遍历pstDnsInfo中存储的DNS信息 */
    for (ucIndex = 0; ucIndex < IMS_NIC_MAX_DNS_SERVER_NUM; ucIndex++)
    {
        if ( pstDnsInfo->astImsNicDnsInfo[ucIndex].cDnsRef > 0)
        {
            if (pstDnsInfo->astImsNicDnsInfo[ucIndex].ucIpAddrLen == ucIpAddrLen)
            {
                /*  比较pucIpAddr和pstDnsInfo->astImsNicDnsInfo[ucIndex].aucDnsAddr的内存(长度ucIpAddrLen) */
                if ( IMS_NIC_IsSameIpAddr(pucIpAddr, pstDnsInfo->astImsNicDnsInfo[ucIndex].aucDnsAddr, ucIpAddrLen) )
                {
                    /* 取当前索引 */
                    *pucDnsIndx  = ucIndex;

                    return IMS_NIC_DNS_CFGED;
                }
            }
        }
        else
        {
            /* 记录当前空闲的DNS编号 */
            aucIdle[ucIdleNum]   = ucIndex;
            ucIdleNum++;
        }
    }

    /* 取第一个空闲索引 */
    *pucDnsIndx  = aucIdle[0];

    return IMS_NIC_DNS_NOT_CFGED;

}

/*****************************************************************************
 函 数 名  : IMS_NIC_DnsConfig
 功能描述  : 配置DNS服务器
 输入参数  : pucIpAddr              -- 欲配置的DNS服务器IP
             ulIpAddrLen            -- IP地址长度
             pstImsNicDnsInfo       -- 内部存储的DNS配置信息
             pstImsNicPdnInfo       -- 内部存储的对应承载上虚拟网卡的配置信息
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为对应的错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_DnsConfig (VOS_UINT8 *pucIpAddr, VOS_UINT8 ucIpAddrLen, IMS_NIC_DNS_INFO_STRU *pstDnsInfo, IMS_NIC_SINGLE_PDN_INFO_STRU *pstSinglePdnInfo)
{
    VOS_UINT8               ucDnsIndx;
    VOS_UINT32              ulResult;
    VOS_UINT8               aucIpAddrStr[IMS_NIC_IPV6_ADDR_STR_LEN + 1] = {0};
    VOS_INT                 iLen;


    ucDnsIndx = 0;

    if (VOS_NULL_PTR == pucIpAddr)
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DnsConfig, ERROR, pucIpAddr is Null Ptr.");

        return IMS_NIC_CONFIG_PTR_NULL;
    }

    if (VOS_NULL_PTR == pstDnsInfo)
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DnsConfig, ERROR, pstDnsInfo is Null Ptr.");

        return IMS_NIC_CONFIG_PTR_NULL;
    }

    if (VOS_NULL_PTR == pstSinglePdnInfo)
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DnsConfig, ERROR, pstSinglePdnInfo is Null Ptr.");

        return IMS_NIC_CONFIG_PTR_NULL;
    }

    /* 判断当前欲配置的DNS服务器是否已经配置过 */
    ulResult     = IMS_NIC_DnsAddrCheck(pucIpAddr, ucIpAddrLen, pstDnsInfo, &ucDnsIndx);

    /* 该DNS服务器没有配置过 */
    if ( IMS_NIC_DNS_CFGED != ulResult )
    {
        if ( pstDnsInfo->ucDnsServerCnt < IMS_NIC_MAX_DNS_SERVER_NUM )
        {
            if (IMS_NIC_IPV6_ADDR_LENGTH == ucIpAddrLen)
            {
                /* IPv6地址转为字符串 */
                IMS_NIC_ConvertIpv6AddrToCompressedStr(aucIpAddrStr, pucIpAddr, IMS_NIC_IPV6_STR_RFC2373_TOKENS);

                /* 根据Indx配置DNS Server到Interpeak里面 */
                IMS_NIC_IPCOM_DNSS_CONFIG(ucDnsIndx, aucIpAddrStr);
            }
            else
            {
                /* IPv4地址转为字符串 */
                iLen = IMS_NIC_IP4_TO_STR(aucIpAddrStr, pucIpAddr);
                IMS_NIC_SET_CMD_END(aucIpAddrStr, iLen);

                /* 根据Indx配置DNS Server到Interpeak里面 */
                IMS_NIC_IPCOM_DNSS_CONFIG(ucDnsIndx, aucIpAddrStr);
            }

            DRV_RT_MEMCPY(pstDnsInfo->astImsNicDnsInfo[ucDnsIndx].aucDnsAddr, pucIpAddr, ucIpAddrLen);
            pstDnsInfo->astImsNicDnsInfo[ucDnsIndx].ucIpAddrLen  = ucIpAddrLen;
            pstDnsInfo->ucDnsServerCnt++;
        }
        else
        {
            IMS_NIC_STATS_INC(IMS_NIC_STATS_CFG_DNS_SERVER_FULL, 1, IMS_NIC_MODEM_ID_0);

            return IMS_NIC_DNS_SERVER_FULL;
        }
    }

    /* 如果服务器已经配置过，或配置成功将引用加上 */
    pstSinglePdnInfo->aucDnsFlag[ucDnsIndx]  = VOS_TRUE;

    /* 将该DNS服务器对应的引用计数加1 */
    pstDnsInfo->astImsNicDnsInfo[ucDnsIndx].cDnsRef++;

    return IMS_NIC_SUCC;
}



VOS_UINT32 IMS_NIC_RecfgUncfgedDnsIpv4(IMS_NIC_SINGLE_PDN_INFO_STRU *pstSinglePdnInfo, IMS_NIC_DNS_INFO_STRU *pstDnsInfo)
{
    VOS_UINT32 ulRst;


    ulRst = IMS_NIC_SUCC;

    if (VOS_NULL_PTR == pstSinglePdnInfo || VOS_NULL_PTR == pstDnsInfo)
    {
        IMS_NIC_WARNING_LOG(UEPS_PID_IMSNIC, "IMS_NIC_RecfgUncfgedDns, ERROR, Input para is NULL.");

        return IMS_NIC_CONFIG_PTR_NULL;
    }

    if ((VOS_TRUE == pstSinglePdnInfo->stIpv4PdnInfo.bitOpDnsPrim)
        && (VOS_FALSE == pstSinglePdnInfo->bitOpIpv4DnsPrimCfged))
    {
        /* 配置DNS到Interpeak中 */
        ulRst = IMS_NIC_IPV4_DNS_CONFIG(pstSinglePdnInfo->stIpv4PdnInfo.aucDnsPrimAddr, pstDnsInfo, pstSinglePdnInfo);

        /* 配置成功表示该DNS已经被配置到Interpeak中,否则因为DNS已经配置满，当前DNS未配置到Interpeak */
        pstSinglePdnInfo->bitOpIpv4DnsPrimCfged = ((IMS_NIC_SUCC == ulRst) ? VOS_TRUE : VOS_FALSE);
    }

    if ((VOS_TRUE == pstSinglePdnInfo->stIpv4PdnInfo.bitOpDnsSec)
        && (VOS_FALSE == pstSinglePdnInfo->bitOpIpv4DnsSecCfged))
    {
        /* 配置DNS到Interpeak中 */
        ulRst = IMS_NIC_IPV4_DNS_CONFIG(pstSinglePdnInfo->stIpv4PdnInfo.aucDnsSecAddr, pstDnsInfo, pstSinglePdnInfo);

        /* 配置成功表示该DNS已经被配置到Interpeak中,否则因为DNS已经配置满，当前DNS未配置到Interpeak */
        pstSinglePdnInfo->bitOpIpv4DnsSecCfged = ((IMS_NIC_SUCC == ulRst) ? VOS_TRUE : VOS_FALSE);
    }

    return ulRst;
}



VOS_UINT32 IMS_NIC_RecfgUncfgedDnsIpv6(IMS_NIC_SINGLE_PDN_INFO_STRU *pstSinglePdnInfo, IMS_NIC_DNS_INFO_STRU *pstDnsInfo)
{
    VOS_UINT32 ulRst;


    ulRst = IMS_NIC_SUCC;

    if (VOS_NULL_PTR == pstSinglePdnInfo || VOS_NULL_PTR == pstDnsInfo)
    {
        IMS_NIC_WARNING_LOG(UEPS_PID_IMSNIC, "IMS_NIC_RecfgUncfgedDns, ERROR, Input para is NULL.");

        return IMS_NIC_CONFIG_PTR_NULL;
    }

    if ((VOS_TRUE == pstSinglePdnInfo->stIpv6PdnInfo.bitOpDnsPrim)
        && (VOS_FALSE == pstSinglePdnInfo->bitOpIpv6DnsPrimCfged))
    {
        /* 配置DNS到Interpeak中 */
        ulRst = IMS_NIC_IPV6_DNS_CONFIG(pstSinglePdnInfo->stIpv6PdnInfo.aucDnsPrimAddr, pstDnsInfo, pstSinglePdnInfo);

        /* 配置成功表示该DNS已经被配置到Interpeak中,否则因为DNS已经配置满，当前DNS未配置到Interpeak */
        pstSinglePdnInfo->bitOpIpv6DnsPrimCfged = ((IMS_NIC_SUCC == ulRst) ? VOS_TRUE : VOS_FALSE);
    }

    if ((VOS_TRUE == pstSinglePdnInfo->stIpv6PdnInfo.bitOpDnsSec)
        && (VOS_FALSE == pstSinglePdnInfo->bitOpIpv6DnsSecCfged))
    {
        /* 配置DNS到Interpeak中 */
        ulRst = IMS_NIC_IPV6_DNS_CONFIG(pstSinglePdnInfo->stIpv6PdnInfo.aucDnsSecAddr, pstDnsInfo, pstSinglePdnInfo);

        /* 配置成功表示该DNS已经被配置到Interpeak中,否则因为DNS已经配置满，当前DNS未配置到Interpeak */
        pstSinglePdnInfo->bitOpIpv6DnsSecCfged = ((IMS_NIC_SUCC == ulRst) ? VOS_TRUE : VOS_FALSE);
    }

    return ulRst;
}



VOS_VOID  IMS_NIC_RecfgUncfgedDns(IMS_NIC_PDN_INFO_STRU *pstPdnInfo)
{
    VOS_UINT32                      ulIndx;
    IMS_NIC_SINGLE_PDN_INFO_STRU   *pstSinglePdnInfo;


    if (VOS_NULL_PTR == pstPdnInfo)
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_RecfgUncfgedDns, ERROR, Input para is NULL.");

        return;
    }

    for(ulIndx = 0; ulIndx < IMS_NIC_RAB_ID_MAX_NUM; ulIndx++)
    {
        pstSinglePdnInfo = &pstPdnInfo->astImsNicPdnInfo[ulIndx];

        if (VOS_TRUE == pstSinglePdnInfo->bitOpIpv4PdnInfo)
        {
            /* 一旦不成功就不用再找了,退出循环 */
            if (IMS_NIC_SUCC != IMS_NIC_RecfgUncfgedDnsIpv4(pstSinglePdnInfo, &pstPdnInfo->stImsNicDnsInfo))
            {
                break;
            }
        }

        if (VOS_TRUE == pstSinglePdnInfo->bitOpIpv6PdnInfo)
        {
            /* 一旦不成功就不用再找了,退出循环 */
            if (IMS_NIC_SUCC != IMS_NIC_RecfgUncfgedDnsIpv6(pstSinglePdnInfo, &pstPdnInfo->stImsNicDnsInfo))
            {
                break;
            }
        }
    }
}


VOS_VOID IMS_NIC_DnsInfoDel(IMS_NIC_PDN_INFO_STRU *pstPdnInfo, VOS_UINT8 ucRabId)
{
    VOS_UINT32                           ulIndx;
    IMS_NIC_SINGLE_PDN_INFO_STRU        *pstSinglePdnInfo;
    IMS_NIC_DNS_INFO_STRU               *pstDnsInfo;


    if (VOS_NULL_PTR == pstPdnInfo)
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DnsInfoDel, ERROR, Input para is NULL.");

        return;
    }

    pstSinglePdnInfo = &(pstPdnInfo->astImsNicPdnInfo[ucRabId - IMS_NIC_RAB_ID_OFFSET]);
    pstDnsInfo       = &(pstPdnInfo->stImsNicDnsInfo);

    for (ulIndx = 0; ulIndx < IMS_NIC_MAX_DNS_SERVER_NUM; ulIndx++)
    {
        /* 找到对应的dns索引 */
        if (VOS_TRUE == pstSinglePdnInfo->aucDnsFlag[ulIndx])
        {
            pstDnsInfo->astImsNicDnsInfo[ulIndx].cDnsRef--;
            pstSinglePdnInfo->aucDnsFlag[ulIndx]    = VOS_FALSE;

            /* 如果当前DNS已经没有人引用，从Interpeak中删除 */
            if (0 >= pstDnsInfo->astImsNicDnsInfo[ulIndx].cDnsRef)
            {
                IMS_NIC_IPCOM_DNSS_DEL(ulIndx);

                PS_MEM_SET(&pstDnsInfo->astImsNicDnsInfo[ulIndx], 0, sizeof(IMS_NIC_SINGLE_DNS_INFO_STRU));
                pstDnsInfo->ucDnsServerCnt--;
            }
        }
    }

    /* 如果有从Interpeak中删除DNS Server，查找之前的配置看是否有未配置成功的DNS,将其配到Interpeak中 */
    if (pstDnsInfo->ucDnsServerCnt < IMS_NIC_MAX_DNS_SERVER_NUM)
    {
        IMS_NIC_RecfgUncfgedDns(pstPdnInfo);
    }
}


/*****************************************************************************
 函 数 名  : IMS_NIC_PdnInfoDelIpv4
 功能描述  : IMS虚拟网卡PDN IPv4配置信息删除
 输入参数  : ucRabId        承载号
             enModemId      当前modemID

 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID   IMS_NIC_PdnInfoDelIpv4(VOS_UINT8 ucRabId, MODEM_ID_ENUM_UINT16 enModemId)
{
    VOS_UINT8                           aucCmd[IMS_NIC_CMD_LEN] = {0};
    VOS_INT                             iLen;
    IMS_NIC_ENTITY_STRU                *pstImsNicEntity;
    IMS_NIC_SINGLE_PDN_INFO_STRU       *pstSinglePdnInfo;
    IMS_NIC_PDN_INFO_STRU              *pstPdnInfo;


    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(enModemId);
    pstPdnInfo          = &(pstImsNicEntity->stImsNicPdnInfo);
    pstSinglePdnInfo    = &(pstPdnInfo->astImsNicPdnInfo[ucRabId - IMS_NIC_RAB_ID_OFFSET]);


    /* 删除IP地址 */
    iLen = IMS_NIC_DEL_IPV4_CMD(aucCmd, enModemId, IMS_NIC_GET_NIC_NAME, pstSinglePdnInfo->stIpv4PdnInfo.aucIpV4Addr);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    /* 先清状态，再清除记录的IP信息 */
    pstSinglePdnInfo->bitOpIpv4PdnInfo      = VOS_FALSE;
    pstSinglePdnInfo->bitOpIpv4DnsPrimCfged = VOS_FALSE;
    pstSinglePdnInfo->bitOpIpv4DnsSecCfged  = VOS_FALSE;

    PS_MEM_SET(&(pstSinglePdnInfo->stIpv4PdnInfo), 0, sizeof(IMS_NIC_IPV4_PDN_INFO_STRU));
}

/*****************************************************************************
 函 数 名  : IMS_NIC_PdnInfoDelIpv6
 功能描述  : IMS虚拟网卡PDN IPv6配置信息删除
 输入参数  : ucRabId        承载号
             enModemId      当前modemID

 输出参数  : 无
 返 回 值  : VOS_VOID
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_VOID   IMS_NIC_PdnInfoDelIpv6(VOS_UINT8 ucRabId, MODEM_ID_ENUM_UINT16 enModemId)
{
    VOS_UINT8                           aucCmd[IMS_NIC_CMD_LEN] = {0};
    VOS_UINT8                           aucIpAddrStr[IMS_NIC_IPV6_ADDR_STR_LEN + 1] = {0};
    VOS_INT                             iLen;
    IMS_NIC_ENTITY_STRU                *pstImsNicEntity;
    IMS_NIC_SINGLE_PDN_INFO_STRU       *pstSinglePdnInfo;
    IMS_NIC_PDN_INFO_STRU              *pstPdnInfo;


    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(enModemId);
    pstPdnInfo          = &(pstImsNicEntity->stImsNicPdnInfo);
    pstSinglePdnInfo    = &(pstPdnInfo->astImsNicPdnInfo[ucRabId - IMS_NIC_RAB_ID_OFFSET]);

    IMS_NIC_ConvertIpv6AddrToCompressedStr((VOS_UINT8 *)aucIpAddrStr, pstSinglePdnInfo->stIpv6PdnInfo.aucIpV6Addr, IMS_NIC_IPV6_STR_RFC2373_TOKENS);

    /* 删除IP地址 */
    iLen = IMS_NIC_DEL_IPV6_CMD(aucCmd, enModemId, IMS_NIC_GET_NIC_NAME, aucIpAddrStr);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    /* 先清状态，再清除记录的IP信息 */
    pstSinglePdnInfo->bitOpIpv6PdnInfo      = VOS_FALSE;
    pstSinglePdnInfo->bitOpIpv6DnsPrimCfged = VOS_FALSE;
    pstSinglePdnInfo->bitOpIpv6DnsSecCfged  = VOS_FALSE;

    PS_MEM_SET(&(pstSinglePdnInfo->stIpv6PdnInfo), 0, sizeof(IMS_NIC_IPV6_PDN_INFO_STRU));
}


/*****************************************************************************
 函 数 名  : IMS_NIC_PdnInfoDel
 功能描述  : IMS虚拟网卡PDN配置信息删除，由IMSA模块在PDN去激活后调用
 输入参数  : ucRabId        承载号
             enModemId      当前modemID
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为对应的错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_PdnInfoDel(VOS_UINT8 ucRabId, MODEM_ID_ENUM_UINT16 enModemId)
{
    IMS_NIC_ENTITY_STRU                 *pstImsNicEntity;
    IMS_NIC_SINGLE_PDN_INFO_STRU        *pstSinglePdnInfo;
    IMS_NIC_PDN_INFO_STRU               *pstPdnInfo;


    IMS_NIC_MNTN_TRACE_NO_DATA(ID_IMS_NIC_MNTN_TRACE_RCV_CONFIG_DEL, enModemId, ucRabId);

    if ( enModemId >= IMS_NIC_MODEM_ID_BUTT )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_CFG_DEL_MODEM_ID_ERR, 1, IMS_NIC_MODEM_ID_0);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoDel, ERROR Input modem id invalid!\n", enModemId);

        return IMS_NIC_MODEM_ID_INVALID;
    }

    /* 检查承载号是否有效，取值范围[5, 15] */
    if ( !IMS_NIC_IS_RAB_VALID(ucRabId) )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_CFG_DEL_RAB_ID_ERR, 1, enModemId);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoDel, ERROR Input Rab id invalid!\n", ucRabId);

        return IMS_NIC_RABID_INVALID;
    }


    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(enModemId);
    pstPdnInfo          = &(pstImsNicEntity->stImsNicPdnInfo);
    pstSinglePdnInfo    = &(pstPdnInfo->astImsNicPdnInfo[ucRabId - IMS_NIC_RAB_ID_OFFSET]);

    if (VOS_FALSE == (pstSinglePdnInfo->bitOpIpv4PdnInfo | pstSinglePdnInfo->bitOpIpv6PdnInfo))
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_CFG_DEL_NO_PDN, 1, enModemId);

        IMS_NIC_WARNING_LOG2(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoDel, ERROR No Cfged PDN!\n", enModemId, ucRabId);

        return IMS_NIC_NO_CFG_PDN;
    }

    if (0 >= pstPdnInfo->iPdnCfgCnt)
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_NO_CFGED_PDN, 1, enModemId);

        IMS_NIC_WARNING_LOG2(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoDel, ERROR Input Rab id invalid!\n", enModemId, ucRabId);

        /* 该场景可能异常，清一下状态 */
        IMS_NIC_ResetAll(enModemId);

        return IMS_NIC_NO_CFG_PDN;
    }


    if ( VOS_TRUE == pstSinglePdnInfo->bitOpIpv4PdnInfo )
    {
       IMS_NIC_PdnInfoDelIpv4(ucRabId, enModemId);
    }

    if ( VOS_TRUE == pstSinglePdnInfo->bitOpIpv6PdnInfo )
    {
        IMS_NIC_PdnInfoDelIpv6(ucRabId, enModemId);
    }

    IMS_NIC_DnsInfoDel(pstPdnInfo, ucRabId);

    pstPdnInfo->iPdnCfgCnt--;

    if (0 >= pstPdnInfo->iPdnCfgCnt)
    {
        IMS_NIC_ResetAll(enModemId);
    }

    IMS_NIC_MNTN_TRACE2(ID_IMS_NIC_MNTN_TRACE_RCV_CONFIG_DEL_RSLT, pstPdnInfo, sizeof(IMS_NIC_PDN_INFO_STRU), enModemId, ucRabId);

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_PdnInfoConfigIpv4
 功能描述  : IMS虚拟网卡IPv4配置接口
 输入参数  : pstConfigInfo   IMS虚拟网卡配置信息
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为对应的错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_PdnInfoConfigIpv4(IMS_NIC_PDN_INFO_CONFIG_STRU *pstConfigInfo)
{
    VOS_UINT8                           aucCmd[IMS_NIC_CMD_LEN] = {0};
    VOS_UINT32                          ulRst;
    VOS_INT                             iLen;
    IMS_NIC_ENTITY_STRU                *pstImsNicEntity;
    IMS_NIC_SINGLE_PDN_INFO_STRU       *pstSinglePdnInfo;
    IMS_NIC_DNS_INFO_STRU              *pstDnsInfo;


    ulRst = IMS_NIC_SUCC;

    if (VOS_NULL_PTR == pstConfigInfo)
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoConfigIpv4, ERROR, Input Null Ptr.");

        return IMS_NIC_CONFIG_PTR_NULL;
    }

    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(pstConfigInfo->enModemId);
    pstSinglePdnInfo    = &(pstImsNicEntity->stImsNicPdnInfo.astImsNicPdnInfo[pstConfigInfo->ucRabId  - IMS_NIC_RAB_ID_OFFSET]);
    pstDnsInfo          = &(pstImsNicEntity->stImsNicPdnInfo.stImsNicDnsInfo);

    pstSinglePdnInfo->bitOpIpv4PdnInfo  = VOS_TRUE;
    DRV_RT_MEMCPY(pstSinglePdnInfo->stIpv4PdnInfo.aucIpV4Addr, pstConfigInfo->stIpv4PdnInfo.aucIpV4Addr, IMS_NIC_IPV4_ADDR_LENGTH);

    /* 配置ipv4网卡 */
    iLen = IMS_NIC_ADD_IPV4_CMD(aucCmd, pstConfigInfo->enModemId, IMS_NIC_GET_NIC_NAME, pstConfigInfo->stIpv4PdnInfo.aucIpV4Addr);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    if (VOS_TRUE == pstConfigInfo->stIpv4PdnInfo.bitOpDnsPrim)
    {
        /* 保存DNS地址信息 */
        DRV_RT_MEMCPY(pstSinglePdnInfo->stIpv4PdnInfo.aucDnsPrimAddr, pstConfigInfo->stIpv4PdnInfo.aucDnsPrimAddr, IMS_NIC_IPV4_ADDR_LENGTH);
        pstSinglePdnInfo->stIpv4PdnInfo.bitOpDnsPrim = VOS_TRUE;

        /* 配置DNS到Interpeak中 */
        ulRst = IMS_NIC_IPV4_DNS_CONFIG(pstConfigInfo->stIpv4PdnInfo.aucDnsPrimAddr, pstDnsInfo, pstSinglePdnInfo);

        /* 配置成功表示该DNS已经被配置到Interpeak中,否则因为DNS已经配置满，当前DNS未配置到Interpeak */
        pstSinglePdnInfo->bitOpIpv4DnsPrimCfged = ((IMS_NIC_SUCC == ulRst) ? VOS_TRUE : VOS_FALSE);
    }

    if (VOS_TRUE == pstConfigInfo->stIpv4PdnInfo.bitOpDnsSec)
    {
        /* 保存DNS地址信息 */
        DRV_RT_MEMCPY(pstSinglePdnInfo->stIpv4PdnInfo.aucDnsSecAddr, pstConfigInfo->stIpv4PdnInfo.aucDnsSecAddr, IMS_NIC_IPV4_ADDR_LENGTH);
        pstSinglePdnInfo->stIpv4PdnInfo.bitOpDnsSec = VOS_TRUE;

        /* 配置DNS到Interpeak中 */
        ulRst = IMS_NIC_IPV4_DNS_CONFIG(pstConfigInfo->stIpv4PdnInfo.aucDnsSecAddr, pstDnsInfo, pstSinglePdnInfo);

        /* 配置成功表示该DNS已经被配置到Interpeak中,否则因为DNS已经配置满，当前DNS未配置到Interpeak */
        pstSinglePdnInfo->bitOpIpv4DnsSecCfged = ((IMS_NIC_SUCC == ulRst) ? VOS_TRUE : VOS_FALSE);
    }

    return ulRst;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_PdnInfoConfigIpv6
 功能描述  : IMS虚拟网卡IPv6配置接口
 输入参数  : pstConfigInfo   IMS虚拟网卡配置信息
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为对应的错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_PdnInfoConfigIpv6(IMS_NIC_PDN_INFO_CONFIG_STRU *pstConfigInfo)
{
    VOS_UINT8                           aucCmd[IMS_NIC_CMD_LEN] = {0};
    VOS_UINT8                           aucIpAddrStr[IMS_NIC_IPV6_ADDR_STR_LEN + 1] = {0};
    VOS_UINT32                          ulRst;
    VOS_INT                             iLen;
    IMS_NIC_ENTITY_STRU                *pstImsNicEntity;
    IMS_NIC_SINGLE_PDN_INFO_STRU       *pstSinglePdnInfo;
    IMS_NIC_DNS_INFO_STRU              *pstDnsInfo;


    ulRst  = IMS_NIC_SUCC;

    if (VOS_NULL_PTR == pstConfigInfo)
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoConfigIpv6, ERROR, Input Null Ptr.");

        return IMS_NIC_CONFIG_PTR_NULL;
    }

    if (pstConfigInfo->stIpv6PdnInfo.ulBitPrefixLen > IMS_NIC_IPV6_ADDR_BIT_LEN)
    {
        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoConfigIpv6, ERROR, Invalid Prefixlen.", pstConfigInfo->stIpv6PdnInfo.ulBitPrefixLen);

        IMS_NIC_STATS_INC(IMS_NIC_STATS_INVALID_IPV6_PREFIX_LEN, 1, pstConfigInfo->enModemId);

        return IMS_NIC_PREFIXLEN_INVALID;
    }

    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(pstConfigInfo->enModemId);
    pstSinglePdnInfo    = &(pstImsNicEntity->stImsNicPdnInfo.astImsNicPdnInfo[pstConfigInfo->ucRabId  - IMS_NIC_RAB_ID_OFFSET]);
    pstDnsInfo          = &(pstImsNicEntity->stImsNicPdnInfo.stImsNicDnsInfo);

    pstSinglePdnInfo->bitOpIpv6PdnInfo  = VOS_TRUE;
    pstSinglePdnInfo->stIpv6PdnInfo.ulBitPrefixLen    = pstConfigInfo->stIpv6PdnInfo.ulBitPrefixLen;
    DRV_RT_MEMCPY(pstSinglePdnInfo->stIpv6PdnInfo.aucIpV6Addr, pstConfigInfo->stIpv6PdnInfo.aucIpV6Addr, IMS_NIC_IPV6_ADDR_LENGTH);

    IMS_NIC_ConvertIpv6AddrToCompressedStr((VOS_UINT8 *)aucIpAddrStr, pstConfigInfo->stIpv6PdnInfo.aucIpV6Addr, IMS_NIC_IPV6_STR_RFC2373_TOKENS);

    /* 配置ipv6网卡 */
    iLen = IMS_NIC_ADD_IPV6_CMD(aucCmd, pstConfigInfo->enModemId, IMS_NIC_GET_NIC_NAME, aucIpAddrStr, pstConfigInfo->stIpv6PdnInfo.ulBitPrefixLen);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    if (VOS_TRUE == pstConfigInfo->stIpv6PdnInfo.bitOpDnsPrim)
    {
        /* 保存DNS地址信息 */
        DRV_RT_MEMCPY(pstSinglePdnInfo->stIpv6PdnInfo.aucDnsPrimAddr, pstConfigInfo->stIpv6PdnInfo.aucDnsPrimAddr, IMS_NIC_IPV6_ADDR_LENGTH);
        pstSinglePdnInfo->stIpv6PdnInfo.bitOpDnsPrim = VOS_TRUE;

        /* 配置主DNS到Interpeak中 */
        ulRst = IMS_NIC_IPV6_DNS_CONFIG(pstConfigInfo->stIpv6PdnInfo.aucDnsPrimAddr, pstDnsInfo, pstSinglePdnInfo);

        /* 配置成功表示该DNS已经被配置到Interpeak中,否则因为DNS已经配置满，当前DNS未配置到Interpeak */
        pstSinglePdnInfo->bitOpIpv6DnsPrimCfged = ((IMS_NIC_SUCC == ulRst) ? VOS_TRUE : VOS_FALSE);
    }

    if (VOS_TRUE == pstConfigInfo->stIpv6PdnInfo.bitOpDnsSec)
    {
        /* 保存DNS地址信息 */
        DRV_RT_MEMCPY(pstSinglePdnInfo->stIpv6PdnInfo.aucDnsSecAddr, pstConfigInfo->stIpv6PdnInfo.aucDnsSecAddr, IMS_NIC_IPV6_ADDR_LENGTH);
        pstSinglePdnInfo->stIpv6PdnInfo.bitOpDnsSec = VOS_TRUE;

        /* 配置辅DNS到Interpeak中 */
        ulRst = IMS_NIC_IPV6_DNS_CONFIG(pstConfigInfo->stIpv6PdnInfo.aucDnsSecAddr, pstDnsInfo, pstSinglePdnInfo);

        /* 配置成功表示该DNS已经被配置到Interpeak中,否则因为DNS已经配置满，当前DNS未配置到Interpeak */
        pstSinglePdnInfo->bitOpIpv6DnsSecCfged = ((IMS_NIC_SUCC == ulRst) ? VOS_TRUE : VOS_FALSE);
    }

    return ulRst;
}
VOS_VOID IMS_NIC_SetDefaultRoute(MODEM_ID_ENUM_UINT16 enModemId)
{
    VOS_UINT8                           aucCmd[IMS_NIC_CMD_LEN] = {0};
    VOS_INT                             iLen;


    /* 配置IPV4默认路由到虚拟网卡上 */
    iLen = IMS_NIC_IPV4_ROUTE_CMD(aucCmd, enModemId, IMS_NIC_GET_NIC_NAME);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    /* 配置IPV6默认路由到虚拟网卡上 */
    iLen = IMS_NIC_IPV6_ROUTE_CMD(aucCmd, enModemId, IMS_NIC_GET_NIC_NAME);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    return;
}


VOS_UINT32 IMS_NIC_SetSocketPort(VOS_UINT16 usMinPort, VOS_UINT16 usMaxPort)
{
    VOS_UINT8                           aucCmd[IMS_NIC_CMD_LEN] = {0};
    VOS_INT                             iLen;


    if (usMinPort > usMaxPort)
    {
        IMS_NIC_ERROR_LOG2(UEPS_PID_IMSNIC,
            "IMS_NIC_SetSocketPort, ERROR, min, max", usMinPort, usMaxPort);
        return IMS_NIC_FAIL;
    }

    /* 配置Socket端口下限 */
    iLen = IMS_NIC_SET_PORT_MIN_CMD(aucCmd, usMinPort);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    /* 配置Socket端口上限 */
    iLen = IMS_NIC_SET_PORT_MAX_CMD(aucCmd, usMaxPort);
    IMS_NIC_SET_CMD_END(aucCmd, iLen);
    IMS_NIC_RUN_CMD(aucCmd);

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_PdnInfoConfig
 功能描述  : IMS虚拟网卡配置接口,由IMSA模块调用
 输入参数  : pstConfigInfo   IMS虚拟网卡配置信息
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为对应的错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_PdnInfoConfig (IMS_NIC_PDN_INFO_CONFIG_STRU *pstConfigInfo)
{
    IMS_NIC_ENTITY_STRU                 *pstImsNicEntity;
    IMS_NIC_SINGLE_PDN_INFO_STRU        *pstSinglePdnInfo;
    IMS_NIC_PDN_INFO_STRU               *pstPdnInfo;
    VOS_UINT32                           ulResult;
    VOS_UINT32                           ulIsNewConfig;


    ulResult        = IMS_NIC_SUCC;
    ulIsNewConfig   = VOS_FALSE;

    if ( VOS_NULL_PTR == pstConfigInfo )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_CFG_INPUT_NULL_PTR, 1, IMS_NIC_MODEM_ID_0);

        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoConfig, ERROR, Input NULL PTR!");

        return IMS_NIC_CONFIG_PTR_NULL;
    }

    /* 可维可测 */
    IMS_NIC_MNTN_TRACE(ID_IMS_NIC_MNTN_TRACE_RCV_CONFIG_INFO, pstConfigInfo, sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU));

    if ( pstConfigInfo->enModemId >= IMS_NIC_MODEM_ID_BUTT )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_CFG_ADD_RAB_ID_ERR, 1, IMS_NIC_MODEM_ID_0);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoConfig, ERROR, Input Modem id invalid!", pstConfigInfo->enModemId);

        return IMS_NIC_MODEM_ID_INVALID;
    }

    /* 检查承载号是否有效，取值范围[5, 15] */
    if ( !IMS_NIC_IS_RAB_VALID(pstConfigInfo->ucRabId) )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_CFG_ADD_MODEM_ID_ERR, 1, pstConfigInfo->enModemId);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoConfig, ERROR Input Rab id invalid!\n", pstConfigInfo->ucRabId);

        return IMS_NIC_RABID_INVALID;
    }

    if (VOS_FALSE == (pstConfigInfo->bitOpIpv4PdnInfo | pstConfigInfo->bitOpIpv6PdnInfo))
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_CFG_ADD_NO_PDN, 1, pstConfigInfo->enModemId);

        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_PdnInfoConfig, ERROR No IPv4 and IPv6 info cfged!\n");

        return IMS_NIC_NO_CFG_PDN;
    }


    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(pstConfigInfo->enModemId);
    pstPdnInfo          = &(pstImsNicEntity->stImsNicPdnInfo);
    pstSinglePdnInfo    = &(pstPdnInfo->astImsNicPdnInfo[pstConfigInfo->ucRabId - IMS_NIC_RAB_ID_OFFSET]);

    /* 检查当前承载上是否还存在旧的无效配置 */
    if ( ((pstConfigInfo->bitOpIpv4PdnInfo) & (pstSinglePdnInfo->bitOpIpv4PdnInfo))
        | ((pstConfigInfo->bitOpIpv6PdnInfo) & (pstSinglePdnInfo->bitOpIpv6PdnInfo)))
    {
        /* 删除该承载上的配置 */
        IMS_NIC_PdnInfoDel(pstConfigInfo->ucRabId, pstConfigInfo->enModemId);
    }

    /* 新配置一个PDN的时候才需要cfgCnt++，这里主要是通过本地保存的状态确认该rab上是否有被配置过 */
    /* 放到删除之后再判断，可以保证该rab上的配置是处于最新状态 */
    if (VOS_FALSE == (pstSinglePdnInfo->bitOpIpv4PdnInfo | pstSinglePdnInfo->bitOpIpv6PdnInfo))
    {
        ulIsNewConfig = VOS_TRUE;
    }

    /* 配置IPV4相关 */
    if ( VOS_TRUE == pstConfigInfo->bitOpIpv4PdnInfo )
    {
        ulResult = IMS_NIC_PdnInfoConfigIpv4(pstConfigInfo);
    }

    /* 配置IPV6相关 */
    if ( VOS_TRUE == pstConfigInfo->bitOpIpv6PdnInfo )
    {
        ulResult |= IMS_NIC_PdnInfoConfigIpv6(pstConfigInfo);
    }

    if (0 == pstImsNicEntity->stImsNicPdnInfo.iPdnCfgCnt)
    {
        /* 配置默认路由 */
        IMS_NIC_SetDefaultRoute(pstConfigInfo->enModemId);

        /* 端口限制仅在网卡第一次激活时配置一次，重配需先删除网卡 */
        if (VOS_TRUE == pstConfigInfo->bitOpSockPortInfo)
        {
            /* 配置Socket端口 */
            ulResult |= IMS_NIC_SetSocketPort(pstConfigInfo->stSockPortInfo.usSockMinPort,
                pstConfigInfo->stSockPortInfo.usSockMaxPort);
        }
    }

    if (VOS_TRUE == ulIsNewConfig)
    {
        pstPdnInfo->iPdnCfgCnt++;
    }

    /* 前面的可维可测是抓的外部配置信息，这里抓取本模块维护的状态信息 */
    IMS_NIC_MNTN_TRACE2(ID_IMS_NIC_MNTN_TRACE_CONFIG_PROC_RSLT, pstPdnInfo, sizeof(IMS_NIC_PDN_INFO_STRU), ulResult, ulIsNewConfig);

    return ulResult;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_MtuSet
 功能描述  : IMS虚拟网卡MTU设置
 输入参数  : ulMtu        最大发送单元
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为对应的错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_MtuSet(VOS_UINT32 ulMtu)
{
    VOS_UINT16                      usIndex;
    IMS_NIC_ENTITY_STRU            *pstImsNicEntity;
    VOS_CHAR                        acCmd[IMS_NIC_CMD_LEN] = {0};
    VOS_INT32                       iLen;


    /* ulMtu长度不在有效范围内 */
    if ( (ulMtu < IMS_NIC_MIN_IPPKT_LEN) || (ulMtu > IMS_NIC_MAX_IPPKT_LEN) )
    {
        /* 默认记录到Modem0的实体中 */;
        IMS_NIC_STATS_INC(IMS_NIC_STATS_MTU_INVALID, 1, IMS_NIC_MODEM_ID_0);

        return IMS_NIC_MTU_INVALID;
    }

    for (usIndex = 0 ; usIndex < IMS_NIC_MODEM_ID_BUTT ; usIndex++)
    {
        pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(usIndex);

        /* 虚拟网卡已经初始化，调用ifconfig命令配置MTU */
        if ( VOS_TRUE == pstImsNicEntity->ulImsNicInitFlag )
        {
            iLen = IMS_NIC_SET_MTU_CMD(acCmd, usIndex, IMS_NIC_GET_NIC_NAME, ulMtu);
            IMS_NIC_SET_CMD_END(acCmd, iLen);
            /* 调用配置命令设置虚拟网卡的MTU */
            IMS_NIC_RUN_CMD(acCmd);
        }

        /* 修改IMS_NIC_MTU_VALUE的值，虚拟网卡初始化时会设置该值为MTU大小 */
        IMS_NIC_SET_MTU(ulMtu);
    }

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_RegUlDataSendCb
 功能描述  : 注册IMS虚拟网卡上行数据发送回调函数，由CDS模块调用
 输入参数  : pFunc        上行数据发送回调接口
             enModemId    当前modemID
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败:IMS_NIC_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_RegUlDataSendCb(IMS_NIC_SEND_UL_DATA_FUNC pFunc, MODEM_ID_ENUM_UINT16 enModemId)
{
    IMS_NIC_ENTITY_STRU     *pstImsNicEntity;


    if ( enModemId >= IMS_NIC_MODEM_ID_BUTT )
    {
        /* 默认记录到Modem0的实体中 */;
        IMS_NIC_STATS_INC(IMS_NIC_STATS_MODEM_ID_ERR_IN_REG_CB, 1, IMS_NIC_MODEM_ID_0);

        return IMS_NIC_MODEM_ID_INVALID;
    }

    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(enModemId);

    /* 该虚拟网卡没有初始化 */
    if ( VOS_TRUE != pstImsNicEntity->ulImsNicInitFlag )
    {
        IMS_NIC_SAVE_UL_DATA_FUNC(enModemId, pFunc);
    }
    else
    {
        pstImsNicEntity->pUlDataSendFunc = pFunc;
    }

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_DlParaCheck
 功能描述  : IMS虚拟网卡下行数据接收参数检查
 输入参数  : pucData      IP包数据
             usLen        IP包长度
             enModemId    当前modemID
 输出参数  :
 返 回 值  : 成功:IMS_NIC_SUCC，失败:IMS_NIC_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_DlParaCheck(VOS_UINT8 *pucData, VOS_UINT16 usLen, MODEM_ID_ENUM_UINT16 enModemId)
{

    if ( enModemId >= IMS_NIC_MODEM_ID_BUTT )
    {
        /* 默认记录到Modem0的实体中 */;
        IMS_NIC_STATS_INC(IMS_NIC_STATS_MODE_ID_ERR_IN_DL_PROC, 1, IMS_NIC_MODEM_ID_0);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_DlParaCheck, ERROR, Input modem id invalid!\n", enModemId);

        return IMS_NIC_FAIL;
    }

    if ( VOS_NULL_PTR == pucData )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_DL_INPUT_NULL_PTR, 1, enModemId);

        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DlParaCheck, ERROR, input pucData is NULL!\n");

        return IMS_NIC_FAIL;
    }

    /* usLen值不在有效值范围内 */
    if ( (usLen < IMS_NIC_MIN_IPPKT_LEN) || (usLen > IMS_NIC_CLUSTER_SIZE))
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_DL_RCV_DATA_LEN_ERR, 1, enModemId);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_DlParaCheck, ERROR, usLen is invalid! usLen = %d\n", usLen);

        return IMS_NIC_FAIL;
    }

    return IMS_NIC_SUCC;

}

/*****************************************************************************
 函 数 名  : IMS_NIC_DlDataRcv
 功能描述  : IMS虚拟网卡下行数据接收接口，提供给CDS调用
 输入参数  : pucData      IP包数据
             usLen        IP包长度
             enModemId    当前modemID
 输出参数  :
 返 回 值  : 成功:IMS_NIC_SUCC，失败:IMS_NIC_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_DlDataRcv(VOS_UINT8 *pucData, VOS_UINT16 usLen, MODEM_ID_ENUM_UINT16 enModemId)
{
    IMS_NIC_DEV_STRU                    *pstImsNicDev;
    VOS_UINT8                            ucIpType;
    M_BLK_ID                             pstMblk;
    VOS_CHAR                            *pcCluster;
    VOS_CHAR                            *pcTmpCluster;
    CL_BLK_ID                            pstClBlk;
    IMS_NIC_ELEMENT_TAB_STRU            *pstImsNicElementTbl;
    VOS_UINT32                           ulResult;


    /* 入参检查 */
    ulResult        = IMS_NIC_DlParaCheck(pucData, usLen, enModemId);
    if ( IMS_NIC_SUCC != ulResult )
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DlDataRcv, ERROR, input para invalid!\n");

        return IMS_NIC_FAIL;
    }

    /* 通过enModemId取得虚拟网卡的数据结构 */
    pstImsNicDev    = IMS_NIC_GET_DEV_STRU_PTR(enModemId);

    /* 调用系统函数netClusterGet获取一块cluster */
    pcCluster = IMS_NIC_CLUSTER_GET(pstImsNicDev->end.pNetPool, pstImsNicDev->pClPoolId);
    if ( VOS_NULL_PTR == pcCluster )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_GET_CLUSTER_FAIL, 1, enModemId);

        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DlDataRcv, ERROR, get cluster fail!\n");

        return IMS_NIC_FAIL;
    }

    pcTmpCluster            = pcCluster;
    pstImsNicElementTbl     = (IMS_NIC_ELEMENT_TAB_STRU *)IMS_NIC_GET_ELEMENT_TABLE(enModemId);

    /* 解析IP包的版本号 */
    ucIpType    = ((pucData[0] >> IMS_NIC_IP_VERSION_POS) & IMS_NIC_IP_VERSION_MASK);

    switch (ucIpType)
    {
        case IMS_NIC_IPV4_VERSION:
            IMS_NIC_STATS_INC(IMS_NIC_STATS_DL_RCV_IPV4_PKT, 1, enModemId);
            DRV_RT_MEMCPY(pcTmpCluster, &(pstImsNicElementTbl->stIpv4Ethhead), IMS_NIC_ETH_HDR_SIZE);
            break;

        case IMS_NIC_IPV6_VERSION:
            IMS_NIC_STATS_INC(IMS_NIC_STATS_DL_RCV_IPV6_PKT, 1, enModemId);
            DRV_RT_MEMCPY(pcTmpCluster, &(pstImsNicElementTbl->stIpv6Ethhead), IMS_NIC_ETH_HDR_SIZE);
            break;

        default:
            IMS_NIC_STATS_INC(IMS_NIC_STATS_DL_RCV_ERR_PKT, 1, enModemId);
            /* 释放cluster内存 */
            IMS_NIC_CLUSTER_FREE(pstImsNicDev->end.pNetPool, (VOS_UINT8 *)pcCluster);

            IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_DlDataRcv, ERROR, ucIpType=%d error!\n", ucIpType);
            return IMS_NIC_FAIL;
    }

    pcTmpCluster    += IMS_NIC_ETH_HDR_SIZE;

    /* 拷贝传入的pucData数据到获取到的cluster内存中 */
    DRV_RT_MEMCPY(pcTmpCluster, pucData, usLen);

    /* 调用系统函数netClBlkGet申请一个ClBlk控制块 */
    pstClBlk = IMS_NIC_CLBLK_GET(pstImsNicDev->end.pNetPool);
    if ( VOS_NULL_PTR == pstClBlk )
    {
        /* 释放cluster内存 */
        IMS_NIC_CLUSTER_FREE(pstImsNicDev->end.pNetPool, (VOS_UINT8 *)pcCluster);
        IMS_NIC_STATS_INC(IMS_NIC_STATS_GET_CLBLK_FAIL, 1, enModemId);

        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DlDataRcv, ERROR, get ClBlk fail!\n");

        return IMS_NIC_FAIL;
    }

    /* 调用系统函数netMblkGet申请一个mBlk控制块 */
    pstMblk = IMS_NIC_MBLK_GET(pstImsNicDev->end.pNetPool);
    if ( VOS_NULL_PTR == pstMblk )
    {
        /* 释放ClBlk控制块内存和cluster内存 */
        IMS_NIC_CLBLK_FREE(pstImsNicDev->end.pNetPool, pstClBlk);
        IMS_NIC_CLUSTER_FREE(pstImsNicDev->end.pNetPool, (VOS_UINT8 *)pcCluster);
        IMS_NIC_STATS_INC(IMS_NIC_STATS_GET_MBLK_FAIL, 1, enModemId);

        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_DlDataRcv, ERROR, get mBlk fail!\n");

        return IMS_NIC_FAIL;
    }

    /* 调用系统函数netClBlkJoin将cluster添加到ClBlk结构中 */
    IMS_NIC_CLBLK_JOIN(pstClBlk, pcCluster, (VOS_INT)(usLen + IMS_NIC_ETH_HDR_SIZE));

    /* 调用系统函数netMblkClJoin将ClBlk添加到mBlk结构中 */
    IMS_NIC_MBLK_JOIN(pstMblk, pstClBlk);

    pstMblk->mBlkHdr.mData   = pcCluster;
    pstMblk->mBlkHdr.mFlags |= M_PKTHDR;
    pstMblk->mBlkHdr.mLen    = (VOS_UINT32)(usLen + IMS_NIC_ETH_HDR_SIZE);
    pstMblk->mBlkPktHdr.len  = (VOS_UINT32)(usLen + IMS_NIC_ETH_HDR_SIZE);

    /* 调用上层协议(IP)的数据接收函数END_RCV_RTN_CALL */
    END_RCV_RTN_CALL(&pstImsNicDev->end, pstMblk);

    /* 下行发送数据统计 */
    IMS_NIC_STATS_INC(IMS_NIC_STATS_DL_SEND_PKT, 1, enModemId);

    /* 更新网卡下行数据接收字节数 */
    IMS_NIC_STATS_INC(IMS_NIC_STATS_DL_RCV_BYTES, usLen, enModemId);

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_UlDataSend
 功能描述  : 该函数负责将MUX层出来的数据交给CDS
 输入参数  : pucData      IP包数据
             usLen        IP包长度
             enModemId    当前modemID
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为IMS_NIC_FAIL
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_UlDataSend(VOS_UINT8 *pucData, VOS_UINT16 usLen, MODEM_ID_ENUM_UINT16 enModemId)
{
    IMS_NIC_ENTITY_STRU                     *pstImsNicEntity;
    IMS_NIC_ETH_HEADER_STRU                 *pstEthHdr;
    VOS_UINT8                               *pucIpData;
    VOS_UINT16                               usIpDataLen;
    VOS_UINT32                               ulResult;
    VOS_UINT16                               usEtherType;


    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(enModemId);

    /* 数据指针为空 */
    if ( VOS_NULL_PTR == pucData )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_INPUT_NULL_PTR, 1, enModemId);

        IMS_NIC_ERROR_LOG_MSG(UEPS_PID_IMSNIC, "IMS_NIC_UlDataSend, ERROR, Input data is NULL!\n");

        return IMS_NIC_FAIL;
    }

    /* 数据报文长度不在有效值范围内，即[MIN + OFFSET, MTU + OFFSET],
        其中MIN为最小IP包大小，OFFSET为14个字节(14字节以太网包头长度) */
    if ( (usLen < (IMS_NIC_MIN_IPPKT_LEN + IMS_NIC_ETH_HDR_SIZE)) ||
        (usLen > (IMS_NIC_GET_MTU + IMS_NIC_ETH_HDR_SIZE)) )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_RCV_DATA_LEN_ERR, 1, enModemId);

        IMS_NIC_ERROR_LOG_MSG1(UEPS_PID_IMSNIC, "IMS_NIC_UlDataSend, ERROR, Input data len =%d is invalid!\n", usLen);

        return IMS_NIC_FAIL;
    }

    pstEthHdr   = (IMS_NIC_ETH_HEADER_STRU *)pucData;

    /* 网络序转换为主机序 */
    usEtherType   = VOS_NTOHS(pstEthHdr->usEtherType);

    switch (usEtherType)
    {
        case IMS_NIC_ETHERTYPE_IPV4:
            IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_RCV_IPV4_PKT, 1, enModemId);
            break;

        case IMS_NIC_ETHERTYPE_IPV6:
            IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_RCV_IPV6_PKT, 1, enModemId);
            break;

        /* 网卡配置为NOARP, 收到ARP包不进行递交，直接返回 */
        case IMS_NIC_ETHERTYPE_ARP:
            IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_RCV_ARP_PKT, 1, enModemId);
            return IMS_NIC_SUCC;

        default:
            IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_RCV_ERR_PKT, 1, enModemId);
            IMS_NIC_ERROR_LOG_MSG1(UEPS_PID_IMSNIC, "IMS_NIC_UlDataSend, ERROR, receive err pkt, the usEthType = 0x%x!\n", usEtherType);
            return IMS_NIC_FAIL;
    }

    /* 剥离MAC头及偏移地址 */
    pucIpData           = pucData + IMS_NIC_ETH_HDR_SIZE;
    usIpDataLen         = usLen - IMS_NIC_ETH_HDR_SIZE;

    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(enModemId);

    if ( VOS_NULL_PTR   == pstImsNicEntity->pUlDataSendFunc )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_CB_FUN_NOT_REG, 1, enModemId);

        IMS_NIC_ERROR_LOG_MSG(UEPS_PID_IMSNIC, "IMS_NIC_UlDataSend, ERROR, pUlDataSendFunc is NULL!\n");

        return IMS_NIC_FAIL;
    }

    /* 调用CDS注册的上行数据发送函数 */
    ulResult    = pstImsNicEntity->pUlDataSendFunc(pucIpData, usIpDataLen, enModemId);
    if( IMS_NIC_SUCC != ulResult )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_CALL_CB_FUN_FAIL, 1, enModemId);

        IMS_NIC_ERROR_LOG_MSG(UEPS_PID_IMSNIC, "IMS_NIC_UlDataSend, ERROR, call pUlDataSendFunc fail!\n");

        return IMS_NIC_FAIL;
    }

    /* 上行发送数据统计 */
    IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_SEND_PKT, 1, enModemId);

    /* 更新网卡上行数据发送字节数 */
    IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_SEND_BYTES, usIpDataLen, enModemId);

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_MemInit
 功能描述  : 初始化网络缓存
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为IMS_NIC_FAIL
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_MemInit(IMS_NIC_DEV_STRU *pstImsNicDev)
{
    STATUS             iResult;


    /* 为END对象中的net pool结构分配空间 */
    pstImsNicDev->end.pNetPool  = (NET_POOL_ID)IMS_NIC_MALLOC(sizeof(NET_POOL));
    if ( VOS_NULL_PTR == pstImsNicDev->end.pNetPool )
    {
        IMS_NIC_INIT_LOG_PRINT(UEPS_PID_IMSNIC, "IMS_NIC_MemInit, ERROR, Alloc pNetPool failed!\n");
        return IMS_NIC_FAIL;
    }

    /* 配置mBlk控制块，包括mBlk和clBlk控制块个数以及以及预申请的总内存大小 */
    pstImsNicDev->mclCfg.mBlkNum  = IMS_NIC_MBLK_NUM;
    pstImsNicDev->mclCfg.clBlkNum = IMS_NIC_CLBLK_NUM;
    pstImsNicDev->mclCfg.memSize  = (VOS_INT32)
        ((pstImsNicDev->mclCfg.mBlkNum * (MSIZE + sizeof (long))) +
        (pstImsNicDev->mclCfg.clBlkNum * (CL_BLK_SZ + sizeof(long))));

    /* 为mBlk控制块分配空间 */
    pstImsNicDev->mclCfg.memArea =
        (VOS_CHAR *)IMS_NIC_MEMALIGN(sizeof(long), (VOS_UINT32)pstImsNicDev->mclCfg.memSize);
    if ( VOS_NULL_PTR == pstImsNicDev->mclCfg.memArea )
    {
        IMS_NIC_FREE(pstImsNicDev->end.pNetPool);
        IMS_NIC_INIT_LOG_PRINT(UEPS_PID_IMSNIC, "IMS_NIC_MemInit, ERROR, Failed to allocate memory for M_CL_CONFIG struct!\n");
        return IMS_NIC_FAIL;
    }

    /* 配置Cluster控制块，包括块数、大小以及预申请的总内存大小 */
    pstImsNicDev->clDesc.clSize  = IMS_NIC_CLUSTER_SIZE;
    pstImsNicDev->clDesc.clNum   = IMS_NIC_CLUSTER_NUM;
    pstImsNicDev->clDesc.memSize = (VOS_INT32)(pstImsNicDev->clDesc.clNum * (IMS_NIC_CLUSTER_SIZE + 8) + sizeof(int));

    /* 为Cluster控制块分配空间，大小为pstImsNicDev->clDesc.memSize */
    pstImsNicDev->clDesc.memArea = IMS_NIC_MALLOC( (VOS_UINT32)(pstImsNicDev->clDesc.memSize) );
    if ( VOS_NULL_PTR == pstImsNicDev->clDesc.memArea )
    {
        IMS_NIC_FREE(pstImsNicDev->end.pNetPool);
        IMS_NIC_FREE(pstImsNicDev->mclCfg.memArea);
        IMS_NIC_INIT_LOG_PRINT(UEPS_PID_IMSNIC, "IMS_NIC_MemInit, ERROR, Failed to allocate memory for CL_DESC struct!\n");
        return IMS_NIC_FAIL;
    }

    /* 调用的系统函数netPoolInit初始化缓存池 */
    iResult     = IMS_NETPOOL_INIT(pstImsNicDev->end.pNetPool, &pstImsNicDev->mclCfg, &pstImsNicDev->clDesc);
    if ( IMS_NIC_OK != iResult )
    {
        IMS_NIC_FREE(pstImsNicDev->end.pNetPool);
        IMS_NIC_FREE(pstImsNicDev->mclCfg.memArea);
        IMS_NIC_FREE(pstImsNicDev->clDesc.memArea);
        IMS_NIC_INIT_LOG_PRINT(UEPS_PID_IMSNIC, "IMS_NIC_MemInit, ERROR, Failed to initialize net pool!\n");
        return IMS_NIC_FAIL;
    }

    /* 保存缓存池的ID */
    pstImsNicDev->pClPoolId = IMS_NIC_POOL_ID_GET(pstImsNicDev->end.pNetPool, IMS_NIC_CLUSTER_SIZE);

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_Parse
 功能描述  : 加载设备时调用该函数对设备进行初始化
 输入参数  : pinitString    -- 用于初始化的字符串
 输出参数  : punit          -- 虚拟网卡uint号
 返 回 值  : 成功返回IMS_NIC_SUCC，失败返回IMS_NIC_FAIL
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_Parse(
    VOS_INT                             *punit,
    VOS_CHAR                            *pinitString
)
{
    VOS_CHAR        *pctok;
    VOS_CHAR        *pcHolder;


    pcHolder = VOS_NULL_PTR;

    if ( VOS_NULL_PTR == punit )
    {
        IMS_NIC_INIT_LOG_PRINT(UEPS_PID_IMSNIC, "IMS_NIC_Parse, ERROR, punit is NULL!\n");
        return IMS_NIC_FAIL;
    }

    /* 从pinitString中取得unit number，约定在第一个冒号之前 */
    pctok = strtok_r (pinitString, ":", &pcHolder);
    if ( VOS_NULL_PTR == pctok )
    {
        IMS_NIC_INIT_LOG_PRINT(UEPS_PID_IMSNIC, "IMS_NIC_Parse, ERROR, pctok is NULL!\n");
        return IMS_NIC_FAIL;
    }

    *punit = atoi(pctok);

    IMS_NIC_DEBUG_PRINT("IMS_NIC_Parse: *punit = %d\n", *punit);

    return IMS_NIC_SUCC;

}

/*****************************************************************************
 函 数 名  : IMS_NIC_Start
 功能描述  : 加载设备时调用该函数对设备进行初始化
 输入参数  : pinitString    -- 用于初始化的字符串
             nothing        -- 用于携带一些参数，暂未使用
 输出参数  : 无
 返 回 值  : 失败返回VOS_NULL_PTR,成功返回END_OBJ*
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
END_OBJ* IMS_NIC_Load(VOS_CHAR *pinitString, VOS_VOID *nothing)
{
    IMS_NIC_DEV_STRU                    *pstImsNicDev;
    VOS_UINT32                           ulResult;
    STATUS                               iResult;
    VOS_UINT8                           *pucMacAddr;
    LOCAL NET_FUNCS                     *pstImsNicEndFuncTable;
    VOS_INT                              unit;


    if ( VOS_NULL_PTR == pinitString )
    {
        return VOS_NULL_PTR;
    }

    if ( EOS == pinitString[0] )
    {
        /* 把虚拟网卡的名字拷贝到pinitString中 */;
        bcopy (IMS_NIC_GET_NIC_NAME, pinitString, (VOS_INT)(VOS_StrLen(IMS_NIC_GET_NIC_NAME) + 1));
        return VOS_NULL_PTR;
    }

    /* 解析pinitString并从中获取unit*/
    ulResult    = IMS_NIC_Parse(&unit, pinitString);
    if ( (IMS_NIC_SUCC != ulResult) || (unit >= (VOS_INT)IMS_NIC_MODEM_ID_BUTT) )
    {
        IMS_NIC_INIT_LOG_PRINT1(UEPS_PID_IMSNIC, "IMS_NIC_Load, ERROR, IMS_NIC_Parse failed! unit = %d\n", unit);
        return VOS_NULL_PTR;
    }

    /* 获取虚拟网卡设备空间 */
    pstImsNicDev    = IMS_NIC_GET_DEV_STRU_PTR(unit);
    /* 初始化申请的空间 */
    PS_MEM_SET(pstImsNicDev, 0, sizeof(IMS_NIC_DEV_STRU));

    pstImsNicDev->unit      = unit;

    pstImsNicEndFuncTable   = (NET_FUNCS *)IMS_NIC_GET_END_FUNC_TABLE_PTR;
    /* 调用endLib提供的END对象初始化函数END_OBJ_INIT，设置网卡名、unit以及注册设备驱动表 */
    iResult    =   END_OBJ_INIT(&pstImsNicDev->end,
                                    VOS_NULL_PTR,
                                    IMS_NIC_GET_NIC_NAME,
                                    pstImsNicDev->unit,
                                    pstImsNicEndFuncTable,
                                    "IMS NIC for VoLTE");
    if ( IMS_NIC_OK != iResult )
    {
        IMS_NIC_INIT_LOG_PRINT2(UEPS_PID_IMSNIC, "END_OBJ_INIT, ERROR, END_OBJ_INIT failed! iResult = 0x%x, unit = %d\n", iResult, pstImsNicDev->unit);
        return VOS_NULL_PTR;
    }

    pucMacAddr = (VOS_UINT8*)IMS_NIC_GET_DST_MAC_ADDR(pstImsNicDev->unit);

    /* 调用endLib提供的MIB初始化函数END_MIB_INIT，设置网卡MAC地址和MTU等*/
    iResult    =   END_MIB_INIT( &pstImsNicDev->end,
                                     M2_ifType_ethernet_csmacd,
                                     pucMacAddr,
                                     IMS_NIC_ETHER_ADDR_LEN,
                                     (VOS_INT)IMS_NIC_GET_MTU,
                                     IMS_NIC_SPEED_100M);
    if ( IMS_NIC_OK != iResult )
    {
        IMS_NIC_INIT_LOG_PRINT1(UEPS_PID_IMSNIC, "END_OBJ_INIT, ERROR, END_MIB_INIT failed! iResult = 0x%x\n", iResult);
        return VOS_NULL_PTR;
    }

    /* 初始化IMS虚拟网卡的网络缓存 */
    ulResult    = IMS_NIC_MemInit(pstImsNicDev);
    if ( IMS_NIC_SUCC   != ulResult )
    {
        IMS_NIC_INIT_LOG_PRINT(UEPS_PID_IMSNIC, "IMS_NIC_MemInit, ERROR, IMS_NIC_MemInit failed!\n");
        return VOS_NULL_PTR;
    }

    /* 设置标记，表明设备已经准备好了 */
    END_OBJ_READY(&pstImsNicDev->end, IFF_NOTRAILERS | IFF_NOARP);

    return ((END_OBJ *)pstImsNicDev);
}

/*****************************************************************************
 函 数 名  : IMS_NIC_Init
 功能描述  : IMS虚拟网卡初始化函数
 输入参数  : enModemId      --当前Modem号
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为对应的错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_Init (MODEM_ID_ENUM_UINT16 enModemId)
{
    IMS_NIC_ENTITY_STRU         *pstImsNicEntity;
    IMS_NIC_SEND_UL_DATA_FUNC    pFunc;
    VOS_INT32                   *pulCookie;
    STATUS                       iResult;


    if ( enModemId >= IMS_NIC_MODEM_ID_BUTT )
    {
        /* 默认记录到Modem0的实体中 */;
        IMS_NIC_INIT_LOG_PRINT1(UEPS_PID_IMSNIC, "IMS_NIC_Init, ERROR, Input modem id invalid!\n", enModemId);

        return IMS_NIC_MODEM_ID_INVALID;
    }

    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(enModemId);

    /* 该虚拟网卡已经初始化 */
    if ( VOS_TRUE == pstImsNicEntity->ulImsNicInitFlag )
    {
        IMS_NIC_INIT_LOG_PRINT(UEPS_PID_IMSNIC, "IMS_NIC_Init, WARNNING, IMS NIC is already initialed!\n");
        return IMS_NIC_ALREADY_INITED;
    }

    /* 调用muxDevLoad函数加载IMS虚拟网卡，其中unit传入enModemId，endLoad传入IMS_NIC_Load函数 */
    pulCookie = IMS_NIC_MUX_DEV_LOAD((VOS_INT)enModemId, (VOS_VOID *)IMS_NIC_Load);

    /* 调用muxDevStart函数启动IMS虚拟网卡 */
    iResult   = IMS_NIC_MUX_START(pulCookie);

    if ( IMS_NIC_OK != iResult )
    {
        IMS_NIC_INIT_LOG_PRINT1(UEPS_PID_IMSNIC, "IMS_NIC_Init, ERROR, muxDevStart failed! iResult = %d\n", iResult);
        return IMS_NIC_SYSCALLFAIL;
    }

    /* 初始化虚拟网卡实体pstImsNicEntity */
    PS_MEM_SET(pstImsNicEntity, 0, sizeof(IMS_NIC_ENTITY_STRU));

    pFunc   = IMS_NIC_GET_SAVED_UL_DATA_FUNC(enModemId);

    /* 将保存的上行数据发送回调函数赋值到实体中 */
    pstImsNicEntity->pUlDataSendFunc    = pFunc;

    /* 绑定协议栈到该虚拟网卡上 */
    IMS_NIC_DRV_ETH_INIT(IMS_NIC_GET_NIC_NAME, (VOS_INT)enModemId);

    /* 置上初始化标记 */
    pstImsNicEntity->ulImsNicInitFlag = VOS_TRUE;

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_InitAll
 功能描述  : 初始化所有的IMS虚拟网卡
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为IMS_NIC_FAIL
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_InitAll(VOS_VOID)
{
    VOS_UINT32      ulResult;
    VOS_UINT16      usIndex;


    if (VOS_OK != IMS_NIC_MNTN_LogEntInit(UEPS_PID_IMSNIC))
    {
        IMS_NIC_INIT_LOG_PRINT(UEPS_PID_IMSNIC, "IMS_NIC_InitAll, ERROR, muxDevStart failed! iResult = %d\n");

        return IMS_NIC_FAIL;
    }

    /* 初始化IMS虚拟网卡 */
    for (usIndex = 0 ; usIndex < IMS_NIC_MODEM_ID_BUTT ; usIndex++)
    {
        ulResult    = IMS_NIC_Init(usIndex);
        if ( IMS_NIC_SUCC != ulResult )
        {
            /* 打印出ulResult的值 */
            IMS_NIC_INIT_LOG_PRINT2(UEPS_PID_IMSNIC, "IMS_NIC_InitAll, ERROR, IMS_NIC_Init Failed, usIndex=%d, ulResult=0x%x\n", usIndex, ulResult);
            return IMS_NIC_FAIL;
        }
    }

#ifdef  ST_IMS_NIC
    ST_IMS_NIC_InitQ();
#endif

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_Free
 功能描述  : IMS虚拟网卡去初始化函数
 输入参数  : enModemId      --当前Modem号
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_SUCC，失败为对应的错误码
 调用函数  :
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_Free (MODEM_ID_ENUM_UINT16 enModemId)
{
    IMS_NIC_ENTITY_STRU         *pstImsNicEntity;
    STATUS                       iResult;


    if ( enModemId >= IMS_NIC_MODEM_ID_BUTT )
    {
        /* 默认记录到Modem0的实体中 */;
        IMS_NIC_STATS_INC(IMS_NIC_STATS_MODEM_ID_ERR_OTHER, 1, IMS_NIC_MODEM_ID_0);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_Free, ERROR, Input modem id invalid!\n", enModemId);

        return IMS_NIC_MODEM_ID_INVALID;
    }

    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(enModemId);

    /* 该虚拟网卡没有初始化 */
    if ( VOS_TRUE != pstImsNicEntity->ulImsNicInitFlag )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_NOT_INITED_ERR, 1, enModemId);

        return IMS_NIC_NOT_INITED;
    }

    /* 卸载IMS虚拟网卡，参数分别为网卡名和enModemId */
    iResult     = IMS_NIC_MUX_DEV_UNLOAD(IMS_NIC_GET_NIC_NAME, (VOS_INT)enModemId);
    if ( IMS_NIC_OK != iResult )
    {
        IMS_NIC_STATS_INC(IMS_NIC_STATS_SYS_CALL_FAIL, 1, enModemId);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_Free, ERROR, call muxDevUnload fail!\n", enModemId);

        return IMS_NIC_SYSCALLFAIL;
    }

    pstImsNicEntity->ulImsNicInitFlag = VOS_FALSE;

    return IMS_NIC_SUCC;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_Start
 功能描述  : 该函数用来在设备加载后启动设备
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_Start(IMS_NIC_DEV_STRU *pstImsNicDev)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_Start: IMS NIC Start!\n");

    /* 置虚拟网卡标记为IFF_UP | IFF_RUNNING */
    END_FLAGS_SET(&pstImsNicDev->end, IFF_UP | IFF_RUNNING);

    return IMS_NIC_OK;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_Stop
 功能描述  : 该函数负责停止该虚拟网卡
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_Stop(IMS_NIC_DEV_STRU *pstImsNicDev)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_Stop: IMS NIC Stop!\n");

    /* 清除虚拟网卡Start时置的flags */
    END_FLAGS_CLR(&pstImsNicDev->end, IFF_UP | IFF_RUNNING);

    return IMS_NIC_OK;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_Unload
 功能描述  : 该函数在虚拟网卡卸载时被调用，负责在虚拟网卡卸载时回收设备资源
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_Unload(IMS_NIC_DEV_STRU *pstImsNicDev)
{
    IMS_NIC_ENTITY_STRU                 *pstImsNicEntity;
    IMS_NIC_MODEM_ID_ENUM_UINT16         enModemId;


    IMS_NIC_DEBUG_PRINT("IMS_NIC_Unload entered!\n");

    enModemId   = (IMS_NIC_MODEM_ID_ENUM_UINT16)(pstImsNicDev->unit);

    if ( enModemId >= IMS_NIC_MODEM_ID_BUTT )
    {
        /* 默认记录到Modem0的实体中 */;
        IMS_NIC_STATS_INC(IMS_NIC_STATS_MODEM_ID_ERR_OTHER, 1, IMS_NIC_MODEM_ID_0);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_Unload, ERROR, Input modem id invalid!\n", enModemId);

        return IMS_NIC_ERROR;
    }

    /* 清除虚拟网卡实体 */
    pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(pstImsNicDev->unit);
    PS_MEM_SET(pstImsNicEntity, 0, sizeof(IMS_NIC_ENTITY_STRU));

    /* 清除虚拟网卡上下文记录信息 */
    IMS_NIC_SET_MTU(IMS_NIC_MTU_DEFAULT_VALUE);
    IMS_NIC_SAVE_UL_DATA_FUNC(enModemId, VOS_NULL_PTR);

    /* 清除分配的网络缓存 */
    if ( VOS_NULL_PTR != pstImsNicDev->mclCfg.memArea )
    {
        IMS_NIC_FREE(pstImsNicDev->mclCfg.memArea);
        pstImsNicDev->mclCfg.memArea = VOS_NULL_PTR;
    }

    if ( VOS_NULL_PTR != pstImsNicDev->clDesc.memArea )
    {
        IMS_NIC_FREE(pstImsNicDev->clDesc.memArea);
        pstImsNicDev->clDesc.memArea = VOS_NULL_PTR;
    }

    if ( VOS_NULL_PTR != pstImsNicDev->end.pNetPool )
    {
        IMS_NIC_NETPOOL_DEL(pstImsNicDev->end.pNetPool);
        pstImsNicDev->end.pNetPool  = VOS_NULL_PTR;
    }

    /* 释放虚拟网卡的END对象*/
    END_OBJECT_UNLOAD(&pstImsNicDev->end);

    return IMS_NIC_OK;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_Ioctl
 功能描述  : 该函数负责执行IO控制命令
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
             cmd            -- 命令的ID
             data           -- 命令所带的参数
 输出参数  : 无
 返 回 值  : IMS_NIC_OK表示成功；IMS_NIC_INVAL表示参数无效；IMS_NIC_NOTSUP表示不支持该命令
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL VOS_INT IMS_NIC_Ioctl(IMS_NIC_DEV_STRU *pstImsNicDev, VOS_INT cmd, caddr_t data)
{
    VOS_INT         iResult;
    VOS_INT         iFlag;


    iResult     = IMS_NIC_OK;
    IMS_NIC_DEBUG_PRINT("IMS_NIC_Ioctl: cmd = %d!\n", cmd);

    switch (cmd)
        {
            /* 设置IMS虚拟网卡的MAC地址 */
            case EIOCSADDR:
                if ( VOS_NULL_PTR == data )
                {
                    IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_Ioctl, ERROR, data is NULL!\n");
                    return IMS_NIC_INVAL;
                }

                IMS_NIC_DEBUG_PRINT("IMS_NIC_Ioctl: END_HADDR_LEN = %d!\n", IMS_NIC_END_HADDR_LEN(&pstImsNicDev->end));

                bcopy ((VOS_CHAR *)data,
                       (VOS_CHAR *) IMS_NIC_END_HADDR (&pstImsNicDev->end),
                       IMS_NIC_END_HADDR_LEN(&pstImsNicDev->end));

                break;

            /* 获取IMS虚拟网卡的MAC地址 */
            case EIOCGADDR:
                if ( VOS_NULL_PTR == data )
                {
                    IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_Ioctl, ERROR, data is NULL!\n");
                    return IMS_NIC_INVAL;
                }

                IMS_NIC_DEBUG_PRINT("IMS_NIC_Ioctl: END_HADDR_LEN = %d!\n", IMS_NIC_END_HADDR_LEN(&pstImsNicDev->end));

                bcopy ((VOS_CHAR *)IMS_NIC_END_HADDR (&pstImsNicDev->end), (VOS_CHAR *)data,
                        IMS_NIC_END_HADDR_LEN (&pstImsNicDev->end));
                break;

            /* 获取IMS虚拟网卡的设备标志位flags */
            case EIOCGFLAGS:
                if ( VOS_NULL_PTR == data )
                {
                    IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_Ioctl, ERROR, data is NULL!\n");
                    return IMS_NIC_INVAL;
                }
                *(VOS_INT *)data = END_FLAGS_GET(&pstImsNicDev->end);
                break;

            /* 设置IMS虚拟网卡的设备标志位flags */
            case EIOCSFLAGS:
                iFlag   = (VOS_INT)data;
                if ( iFlag < 0 )
                {
                    iFlag   = -iFlag;
                    iFlag--;
                    END_FLAGS_CLR (&pstImsNicDev->end, iFlag);
                }
                else
                {
                    END_FLAGS_SET (&pstImsNicDev->end, iFlag);
                }
                break;

            /* 获取以太网包头的大小 */
            case EIOCGHDRLEN:
                if ( VOS_NULL_PTR == data )
                {
                    IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_Ioctl, ERROR, data is NULL!\n");
                    return IMS_NIC_INVAL;
                }
                *(VOS_INT *)data = IMS_NIC_ETH_HDR_SIZE;
                break;

            /* 返回 MIB 信息 */
            case EIOCGMIB2233:
            case EIOCGMIB2:
                if( VOS_NULL_PTR == data )
                {
                    IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_Ioctl, ERROR, data is NULL!\n");
                    return IMS_NIC_INVAL;
                }
                bcopy((VOS_CHAR *)&pstImsNicDev->end.mib2Tbl, (VOS_CHAR *)data,
                        sizeof(pstImsNicDev->end.mib2Tbl));
                break;

            default:
                iResult = IMS_NIC_NOTSUP;
            break;
        }

        return iResult;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_MblkCopy
 功能描述  : 该函数负责将不连续的Mblk对应的cluster内存拷贝到连续的内存中
 输入参数  : pstSrcBuf   -- 存储数据的缓存的ID
             pucDstData  -- 目的内存
 输出参数  : 无
 返 回 值  : 返回拷贝的有效长度
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT16 IMS_NIC_MblkCopy(M_BLK_ID pstSrcBuf, VOS_UINT8 *pucDstData)
{
    VOS_UINT16       usDataLen;


    if ( (VOS_NULL_PTR == pstSrcBuf) || (VOS_NULL_PTR == pucDstData) )
    {
        IMS_NIC_ERROR_LOG_MSG(UEPS_PID_IMSNIC, "IMS_NIC_MblkCopy, ERROR, Input para invalid!\n");
        /* 参数错误则返回拷贝的有效长度为0 */
        return 0;
    }

    usDataLen   = 0;

    while ( VOS_NULL_PTR != pstSrcBuf )
    {
        usDataLen   += (VOS_UINT16)(pstSrcBuf->mBlkHdr.mLen);
        if ( usDataLen <= IMS_NIC_UL_SEND_BUFF_SIZE )
        {
            DRV_RT_MEMCPY(pucDstData, (VOS_UINT8 *)(pstSrcBuf->mBlkHdr.mData), (VOS_UINT16)(pstSrcBuf->mBlkHdr.mLen));
            pucDstData  += (VOS_UINT16)(pstSrcBuf->mBlkHdr.mLen);
            pstSrcBuf    = pstSrcBuf->mBlkHdr.mNext;
        }
        else
        {
            IMS_NIC_ERROR_LOG_MSG1(UEPS_PID_IMSNIC, "IMS_NIC_MblkCopy, ERROR, usDataLen = %d invalid!\n", usDataLen);
            return 0;
        }
    }

    return usDataLen;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_Send
 功能描述  : 该函数负责将MUX层出来的数据交给CDS
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
             pBuf           -- 存储数据的缓存的ID
 输出参数  : 无
 返 回 值  : 成功:VOS_OK，失败为VOS_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_Send(IMS_NIC_DEV_STRU *pstImsNicDev, M_BLK_ID pstBuf)
{
    M_BLK_ID                            pstTmpBuf;
    VOS_UINT32                          ulResult;
    VOS_UINT16                          usDataLen;
    VOS_UINT8                          *pucData;
    MODEM_ID_ENUM_UINT16                enModemId;


    pstTmpBuf   = pstBuf;
    enModemId   = (MODEM_ID_ENUM_UINT16)(pstImsNicDev->unit);

    /* pstImsNicDev中记录的unit异常 */
    if ( enModemId >= IMS_NIC_MODEM_ID_BUTT )
    {
        /* 调用系统函数释放该mBlk链式内存 */
        IMS_NIC_MBLK_CHAIN_FREE(pstBuf);

        /* 默认记录到Modem0的实体中 */;
        IMS_NIC_STATS_INC(IMS_NIC_STATS_MODE_ID_ERR_IN_UL_PROC, 1, IMS_NIC_MODEM_ID_0);

        IMS_NIC_ERROR_LOG1(UEPS_PID_IMSNIC, "IMS_NIC_Send, ERROR, Input modem id invalid!\n", enModemId);

        return IMS_NIC_ERROR;
    }

    END_TX_SEM_TAKE(&pstImsNicDev->end, WAIT_FOREVER);

    while ( VOS_NULL_PTR != pstTmpBuf )
    {
        pucData     = (VOS_UINT8 *)(pstTmpBuf->mBlkHdr.mData);
        usDataLen   = (VOS_UINT16)(pstTmpBuf->mBlkHdr.mLen);

        /* 收到的数据包内存不连续，进行数据拷贝 */
        if ( VOS_NULL_PTR != pstTmpBuf->mBlkHdr.mNext )
        {
            IMS_NIC_STATS_INC(IMS_NIC_STATS_UL_PKT_MEM_SEG, 1, enModemId);

            usDataLen   = IMS_NIC_MblkCopy(pstTmpBuf, pstImsNicDev->aucSendBuf);
            if ( 0 == usDataLen )
            {
                /* 继续处理下个数据包 */
                pstTmpBuf = pstTmpBuf->mBlkHdr.mNextPkt;
                IMS_NIC_STATS_INC(IMS_NIC_STATS_MBLK_COPY_ERR, 1, enModemId);
                continue;
            }

            pucData     = pstImsNicDev->aucSendBuf;
        }

        /* 上行数据处理 */
        ulResult    = IMS_NIC_UlDataSend(pucData, usDataLen, enModemId);
        if ( IMS_NIC_SUCC != ulResult )
        {
            IMS_NIC_ERROR_LOG_MSG(UEPS_PID_IMSNIC, "IMS_NIC_Send, ERROR, IMS_NIC_UlDataSend failed!\n");
        }

        pstTmpBuf = pstTmpBuf->mBlkHdr.mNextPkt;
    };

    /* 调用系统函数释放该mBlk链式内存 */
    IMS_NIC_MBLK_CHAIN_FREE(pstBuf);

    END_TX_SEM_GIVE(&pstImsNicDev->end);

    return IMS_NIC_OK;

}

/*****************************************************************************
 函 数 名  : IMS_NIC_MCastAddrAdd
 功能描述  : 在设备上添加一个多播地址
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
             pAddress       -- 地址字符串
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_MCastAddrAdd(IMS_NIC_DEV_STRU *pstImsNicDev, VOS_CHAR *pAddress)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_MCastAddrAdd entered!\n");

    return IMS_NIC_OK;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_MCastAddrDel
 功能描述  : 删除设备上的一个多播地址
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
             pAddress       -- 地址字符串
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_MCastAddrDel(IMS_NIC_DEV_STRU *pstImsNicDev, VOS_CHAR *pAddress)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_MCastAddrDel entered!\n");

    return IMS_NIC_OK;
}


/*****************************************************************************
 函 数 名  : IMS_NIC_MCastAddrGet
 功能描述  : 查询在设备上注册的多播地址列表
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
             pstMultiTable  -- 多播地址列表
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_MCastAddrGet(IMS_NIC_DEV_STRU *pstImsNicDev, MULTI_TABLE *pstMultiTable)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_MCastAddrGet entered!\n");

    return IMS_NIC_OK;
}


/*****************************************************************************
 函 数 名  : IMS_NIC_PollSend
 功能描述  : 设备在轮询模式下的数据发送函数
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
             pstBuf         -- 存储数据的缓存的ID
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_PollSend(IMS_NIC_DEV_STRU *pstImsNicDev, M_BLK_ID pstBuf)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_PollSend entered!\n");

    return IMS_NIC_OK;
}


/*****************************************************************************
 函 数 名  : IMS_NIC_PollRcv
 功能描述  : 设备在轮询模式下的数据接收函数
 输入参数  : pstImsNicDev   -- 指向需要启动的设备实体
             pstBuf         -- 存储数据的缓存的ID
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_PollRcv(IMS_NIC_DEV_STRU *pstImsNicDev, M_BLK_ID pstBuf)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_PollRcv entered!\n");

    return IMS_NIC_OK;
}

#if 0
/*****************************************************************************
 函 数 名  : IMS_NIC_FormAddress
 功能描述  : 为包含着输出数据的mBlk链添加一个帧头,(现直接调用endEtherAddressForm)
 输入参数  :
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL M_BLK_ID IMS_NIC_FormAddress(
    M_BLK_ID                            pNBuff,
    M_BLK_ID                            pSrcAddr,
    M_BLK_ID                            pDstAddr,
    BOOL                                bcastFlag
)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_FormAddress entered!\n");

    return pNBuff;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_PacketDataGet
 功能描述  : 获取报文数据各个部分的偏移,(现直接调用endEtherPacketDataGet)
 输入参数  :
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_PacketDataGet(M_BLK_ID pMblk, LL_HDR_INFO *pLinkHdrInfo)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_PacketDataGet entered!\n");

    /* destination addr offset in mBlk */
    pLinkHdrInfo->destAddrOffset = 0;

    /* destination address size */
    pLinkHdrInfo->destSize       = 0;

    /* source address offset in mBlk */
    pLinkHdrInfo->srcAddrOffset  = 0;

    /* source address size */
    pLinkHdrInfo->srcSize        = 0;

    /* control info offset in mBlk */
    pLinkHdrInfo->ctrlAddrOffset = 0;

    /* control info size */
    pLinkHdrInfo->ctrlSize       = 0;

    /* type of the packet */
    pLinkHdrInfo->pktType        = IMS_NIC_ETHERTYPE_IPV4;

    /* data offset in the mBlk */
    pLinkHdrInfo->dataOffset     = 0;

    return IMS_NIC_OK;
}

/*****************************************************************************
 函 数 名  : IMS_NIC_AddrGet
 功能描述  : 获取报文的地址信息,(现直接调用endEtherPacketAddrGet)
 输入参数  :
 输出参数  : 无
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_ERR
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
LOCAL STATUS IMS_NIC_AddrGet(
    M_BLK_ID                  pMblk,
    M_BLK_ID                  pSrc,
    M_BLK_ID                  pDst,
    M_BLK_ID                  pESrc,
    M_BLK_ID                  pEDst
)
{
    IMS_NIC_DEBUG_PRINT("IMS_NIC_FormAddress entered!\n");

    return IMS_NIC_OK;
}
#endif

/*****************************************************************************
 函 数 名  : IMS_NIC_GetMntnInfo
 功能描述  : 获取IMS虚拟网卡的统计信息，用于周期性上报
 输入参数  : usType         -- 可维可测消息类型
 输出参数  : pucData        -- 存储数据的内存
             pulLen         -- 数据长度
 返 回 值  : 成功:IMS_NIC_OK，失败为IMS_NIC_FAIL
 被调函数  :

 修改历史      :
  1.日    期   : 2013年7月16日
    作    者   : caikai
    修改内容   : 新生成函数

*****************************************************************************/
VOS_UINT32 IMS_NIC_GetMntnInfo
(
    VOS_UINT8                       *pucData,
    VOS_UINT32                      *pulLen,
    VOS_UINT16                       usType
)
{
    VOS_UINT16                      usIndex;
    IMS_NIC_ENTITY_STRU            *pstImsNicEntity;
    VOS_UINT32                      ulModemCnt;


    if ( VOS_NULL_PTR == pucData )
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_GetMntnInfo, ERROR, Input pucData id null!\n");
        return IMS_NIC_FAIL;
    }

    if ( VOS_NULL_PTR == pulLen )
    {
        IMS_NIC_ERROR_LOG(UEPS_PID_IMSNIC, "IMS_NIC_GetMntnInfo, ERROR, Input pulLen id null!\n");
        return IMS_NIC_FAIL;
    }

    *pulLen     = 0;

    /* 填写Modem个数 */
    ulModemCnt  = (VOS_UINT32)IMS_NIC_MODEM_ID_BUTT;
    DRV_RT_MEMCPY(pucData, &ulModemCnt, sizeof(ulModemCnt));
    pucData     += sizeof(ulModemCnt);

    for (usIndex = 0 ; usIndex < IMS_NIC_MODEM_ID_BUTT ; usIndex++)
    {
        pstImsNicEntity     = IMS_NIC_GET_ENTITY_STRU(usIndex);

        /* 填写modemId */
        DRV_RT_MEMCPY(pucData, &usIndex, IMS_NIC_MODEM_ID_LEN);
        pucData     += IMS_NIC_MODEM_ID_OFFSET;
        *pulLen     += IMS_NIC_MODEM_ID_OFFSET;

        /* 拷贝统计信息 */
        DRV_RT_MEMCPY(pucData, (VOS_UINT8 *)(&(pstImsNicEntity->stImsNicStatsInfo)), sizeof(IMS_NIC_STATS_INFO_STRU));
        pucData     += sizeof(IMS_NIC_STATS_INFO_STRU);
        *pulLen     += sizeof(IMS_NIC_STATS_INFO_STRU);
    }

    return IMS_NIC_SUCC;
}
VOS_VOID IMS_NIC_Help(VOS_VOID)
{
    vos_printf("打印网卡状态信息：IMS_NIC_ShowEntityState \n");
    vos_printf("打印网卡统计信息：IMS_NIC_ShowStats \n");
    vos_printf("MuxShow：IMS_NIC_MuxShow \n");
}



VOS_VOID IMS_NIC_ShowEntityState(VOS_VOID)
{
    VOS_UINT8                       ucRabId;
    VOS_INT32                       iIndx1;
    VOS_INT32                       iIndx2;
    VOS_INT32                       iIndx3;
    VOS_UINT16                     *pusIpv6;
    VOS_UINT8                      *pucIpv4;
    IMS_NIC_ENTITY_STRU            *pstImsNicEntity;
    IMS_NIC_SINGLE_DNS_INFO_STRU   *pstSingleDnsInfo;
    IMS_NIC_SINGLE_PDN_INFO_STRU   *pstSinglePdnInfo;


    for (iIndx1 = 0; iIndx1 < IMS_NIC_MODEM_ID_BUTT; iIndx1++)
    {
        pstImsNicEntity = IMS_NIC_GET_ENTITY_STRU(iIndx1);
        vos_printf("======================Entity info for Modem %d ======================\n", iIndx1);
        vos_printf("实体初始化状态:             %d \n", pstImsNicEntity->ulImsNicInitFlag);
        vos_printf("上行发数回调函数指针地址:   0x%x \n", pstImsNicEntity->pUlDataSendFunc);
        vos_printf("当前配置PDN个数:            %d \n", pstImsNicEntity->stImsNicPdnInfo.iPdnCfgCnt);
        vos_printf("当前配置的DNS个数:          %d \n", pstImsNicEntity->stImsNicPdnInfo.stImsNicDnsInfo.ucDnsServerCnt);

        for (iIndx2 = 0; iIndx2 < IMS_NIC_MAX_DNS_SERVER_NUM; iIndx2++)
        {
            pstSingleDnsInfo = &pstImsNicEntity->stImsNicPdnInfo.stImsNicDnsInfo.astImsNicDnsInfo[iIndx2];
            if (IMS_NIC_IPV6_ADDR_LENGTH == pstSingleDnsInfo->ucIpAddrLen)
            {
                pusIpv6 = (VOS_UINT16 *)pstSingleDnsInfo->aucDnsAddr;

                vos_printf("%s 的引用次数:%d, 地址长度:%d, 地址:%x:%x:%x:%x:%x:%x:%x:%x \n",
                            g_stImsNicManageTbl.pcDnsSSysVarName[iIndx2], pstSingleDnsInfo->cDnsRef, pstSingleDnsInfo->ucIpAddrLen,
                            VOS_HTONS(pusIpv6[0]), VOS_HTONS(pusIpv6[1]), VOS_HTONS(pusIpv6[2]), VOS_HTONS(pusIpv6[3]), VOS_HTONS(pusIpv6[4]), VOS_HTONS(pusIpv6[5]), VOS_HTONS(pusIpv6[6]), VOS_HTONS(pusIpv6[7]));
            }
            else if (IMS_NIC_IPV4_ADDR_LENGTH == pstSingleDnsInfo->ucIpAddrLen)
            {
                pucIpv4 = pstSingleDnsInfo->aucDnsAddr;

                vos_printf("%s 的引用次数:%d, 地址长度:%d, 地址:%d.%d.%d.%d \n",
                            g_stImsNicManageTbl.pcDnsSSysVarName[iIndx2], pstSingleDnsInfo->cDnsRef, pstSingleDnsInfo->ucIpAddrLen,
                            pucIpv4[0], pucIpv4[1], pucIpv4[2], pucIpv4[3]);
            }
            else
            {
                vos_printf("%s 的引用次数:%d, 地址长度:%d \n", g_stImsNicManageTbl.pcDnsSSysVarName[iIndx2], pstSingleDnsInfo->cDnsRef, pstSingleDnsInfo->ucIpAddrLen);
            }

        }

        for (iIndx3 = 0; iIndx3 < IMS_NIC_RAB_ID_MAX_NUM; iIndx3++)
        {
            pstSinglePdnInfo = &pstImsNicEntity->stImsNicPdnInfo.astImsNicPdnInfo[iIndx3];

            ucRabId = (VOS_UINT8)iIndx3 + IMS_NIC_RAB_ID_OFFSET;

            vos_printf("PDN %d DNS 配置索引[primaryns]:%d, [secondaryns]:%d, [tertiaryns]:%d, [quaternaryns]:%d \n", ucRabId, pstSinglePdnInfo->aucDnsFlag[0], pstSinglePdnInfo->aucDnsFlag[1], pstSinglePdnInfo->aucDnsFlag[2], pstSinglePdnInfo->aucDnsFlag[3]);
            vos_printf("PDN %d 是否配置IPv4:%d \n", ucRabId, pstSinglePdnInfo->bitOpIpv4PdnInfo);

            if (VOS_TRUE == pstSinglePdnInfo->bitOpIpv4PdnInfo)
            {
                pucIpv4 = pstSinglePdnInfo->stIpv4PdnInfo.aucIpV4Addr;

                vos_printf("PDN %d IPv4地址:%d.%d.%d.%d \n", ucRabId, pucIpv4[0], pucIpv4[1], pucIpv4[2], pucIpv4[3]);
                vos_printf("PDN %d IPv4主DNS信息, bitOpDnsPrim:%d, bitOpDnsPrimCfged:%d \n", ucRabId, pstSinglePdnInfo->stIpv4PdnInfo.bitOpDnsPrim, pstSinglePdnInfo->bitOpIpv4DnsPrimCfged);

                pucIpv4 = pstSinglePdnInfo->stIpv4PdnInfo.aucDnsPrimAddr;
                vos_printf("PDN %d IPv4主DNS地址:%d.%d.%d.%d \n", ucRabId, pucIpv4[0], pucIpv4[1], pucIpv4[2], pucIpv4[3]);

                vos_printf("PDN %d IPv4辅DNS信息, bitOpDnsPrim:%d, bitOpDnsPrimCfged:%d \n", ucRabId, pstSinglePdnInfo->stIpv4PdnInfo.bitOpDnsSec, pstSinglePdnInfo->bitOpIpv4DnsSecCfged);

                pucIpv4 = pstSinglePdnInfo->stIpv4PdnInfo.aucDnsSecAddr;
                vos_printf("PDN %d IPv4主DNS地址:%d.%d.%d.%d \n", ucRabId, pucIpv4[0], pucIpv4[1], pucIpv4[2], pucIpv4[3]);
            }

            vos_printf("PDN %d 是否配置IPv6:%d \n", ucRabId, pstSinglePdnInfo->bitOpIpv6PdnInfo);

            if (VOS_TRUE == pstSinglePdnInfo->bitOpIpv6PdnInfo)
            {
                pusIpv6 = (VOS_UINT16 *)pstSinglePdnInfo->stIpv6PdnInfo.aucIpV6Addr;

                vos_printf("PDN %d IPv6地址:%x:%x:%x:%x:%x:%x:%x:%x \n", ucRabId, VOS_HTONS(pusIpv6[0]), VOS_HTONS(pusIpv6[1]), VOS_HTONS(pusIpv6[2]), VOS_HTONS(pusIpv6[3]), VOS_HTONS(pusIpv6[4]), VOS_HTONS(pusIpv6[5]), VOS_HTONS(pusIpv6[6]), VOS_HTONS(pusIpv6[7]));
                vos_printf("PDN %d IPv6主DNS信息, bitOpDnsPrim:%d, bitOpDnsPrimCfged%d \n", ucRabId, pstSinglePdnInfo->stIpv6PdnInfo.bitOpDnsPrim, pstSinglePdnInfo->bitOpIpv6DnsPrimCfged);

                pusIpv6 = (VOS_UINT16 *)pstSinglePdnInfo->stIpv6PdnInfo.aucDnsPrimAddr;
                vos_printf("PDN %d IPv6主DNS地址:%x:%x:%x:%x:%x:%x:%x:%x \n", ucRabId, VOS_HTONS(pusIpv6[0]), VOS_HTONS(pusIpv6[1]), VOS_HTONS(pusIpv6[2]), VOS_HTONS(pusIpv6[3]), VOS_HTONS(pusIpv6[4]), VOS_HTONS(pusIpv6[5]), VOS_HTONS(pusIpv6[6]), VOS_HTONS(pusIpv6[7]));

                vos_printf("PDN %d IPv6辅DNS信息, bitOpDnsPrim:%d, bitOpDnsPrimCfged:%d \n", ucRabId, pstSinglePdnInfo->stIpv6PdnInfo.bitOpDnsSec, pstSinglePdnInfo->bitOpIpv6DnsSecCfged);

                pusIpv6 = (VOS_UINT16 *)pstSinglePdnInfo->stIpv6PdnInfo.aucDnsSecAddr;
                vos_printf("PDN %d IPv6主DNS地址:%x:%x:%x:%x:%x:%x:%x:%x \n", ucRabId, VOS_HTONS(pusIpv6[0]), VOS_HTONS(pusIpv6[1]), VOS_HTONS(pusIpv6[2]), VOS_HTONS(pusIpv6[3]), VOS_HTONS(pusIpv6[4]), VOS_HTONS(pusIpv6[5]), VOS_HTONS(pusIpv6[6]), VOS_HTONS(pusIpv6[7]));
            }
            vos_printf("\n");
        }
    }
}


VOS_VOID IMS_NIC_ShowStats(VOS_VOID)
{
    VOS_INT32                i;


    for (i = 0; i < IMS_NIC_MODEM_ID_BUTT; i++)
    {
        vos_printf("======================Debug info for Modem %d ======================\n", i);

        IMS_INC_STATS_SHOW(IMS_NIC_STATS_DL_RCV_IPV4_PKT, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_DL_RCV_IPV6_PKT, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_DL_SEND_PKT, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_DL_RCV_BYTES, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_DL_RCV_ERR_PKT, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_DL_INPUT_NULL_PTR, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_DL_RCV_DATA_LEN_ERR, i);

        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_RCV_IPV4_PKT, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_RCV_IPV6_PKT, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_SEND_PKT, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_SEND_BYTES, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_RCV_ERR_PKT, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_RCV_ARP_PKT, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_INPUT_NULL_PTR, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_RCV_DATA_LEN_ERR, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_CB_FUN_NOT_REG, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_CALL_CB_FUN_FAIL, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_UL_PKT_MEM_SEG, i);

        IMS_INC_STATS_SHOW(IMS_NIC_STATS_CFG_DNS_SERVER_FULL, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_CFG_INPUT_NULL_PTR, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_CFG_ADD_MODEM_ID_ERR, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_CFG_DEL_MODEM_ID_ERR, i);

        IMS_INC_STATS_SHOW(IMS_NIC_STATS_CFG_ADD_RAB_ID_ERR, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_CFG_DEL_RAB_ID_ERR, i);

        IMS_INC_STATS_SHOW(IMS_NIC_STATS_GET_CLUSTER_FAIL, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_GET_CLBLK_FAIL, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_GET_MBLK_FAIL, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_MBLK_COPY_ERR, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_NOT_INITED_ERR, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_SYS_CALL_FAIL, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_FIND_DEV_FAIL, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_CFG_ADD_NO_PDN, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_CFG_DEL_NO_PDN, i);

        IMS_INC_STATS_SHOW(IMS_NIC_STATS_MODEM_ID_ERR_OTHER, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_MODEM_ID_ERR_IN_REG_CB, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_MODE_ID_ERR_IN_DL_PROC, i);
        IMS_INC_STATS_SHOW(IMS_NIC_STATS_MODE_ID_ERR_IN_UL_PROC, i);

        IMS_INC_STATS_SHOW(IMS_NIC_STATS_INVALID_IPV6_PREFIX_LEN, i);

        vos_printf("\n\n");
    }
}

VOS_VOID IMS_NIC_MuxShow(VOS_VOID)
{
    muxShow(IMS_NIC_GET_NIC_NAME, 0);

}

#ifdef IMS_NIC_DEBUG
VOS_VOID IMS_NIC_TestSdt(VOS_VOID)
{
    IMS_NIC_PDN_INFO_CONFIG_STRU    stConfigInfo;
    VOS_UINT8                       aucDstIpAddr1[4]            = {0x2, 0x1, 0x15, 0x27};
    VOS_UINT32                      ulResult;
    VOS_UINT8   ucRabId1    = 5;
    char *cmd_str1           = "ifconfig -a";
    char *cmd_str2           = "route -n show";

    VOS_MemSet(&stConfigInfo, 0, sizeof(IMS_NIC_PDN_INFO_CONFIG_STRU));

    stConfigInfo.bitOpIpv4PdnInfo   = PS_TRUE;
    stConfigInfo.bitOpIpv6PdnInfo   = PS_FALSE;
    stConfigInfo.enModemId          = 0;
    stConfigInfo.ucRabId            = ucRabId1;
    VOS_MemCpy(stConfigInfo.stIpv4PdnInfo.aucIpV4Addr, aucDstIpAddr1, 4);

    stConfigInfo.stIpv4PdnInfo.bitOpDnsPrim         = PS_FALSE;
    stConfigInfo.stIpv4PdnInfo.bitOpDnsSec          = PS_FALSE;

    ulResult    = IMS_NIC_PdnInfoConfig (&stConfigInfo);
    if (IMS_NIC_SUCC != ulResult)
    {
        vos_printf("ST_DL_RCV_001: config pdn fail! ulResult = %d\n", ulResult);
        return;
    }

    ipcom_run_cmd(cmd_str1);
    ipcom_run_cmd(cmd_str2);

    /* 删除虚拟网卡配置 */
    IMS_NIC_PdnInfoDel(stConfigInfo.ucRabId, 0);

    ipcom_run_cmd(cmd_str1);
    ipcom_run_cmd(cmd_str2);

    return;
}
#endif

#endif  /*FEATURE_ON == FEATURE_IMS*/


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif



