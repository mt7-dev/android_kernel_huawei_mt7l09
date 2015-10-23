/******************************************************************************

                  版权所有 (C), 2001-2011, 华为技术有限公司

 ******************************************************************************
  文 件 名   : ImsNic.h
  版 本 号   : 初稿
  作    者   : caikai
  生成日期   : 2013年7月16日
  最近修改   :
  功能描述   : IMS虚拟网卡内部头文件
  函数列表   :
  修改历史   :
  1.日    期   : 2013年7月30日
    作    者   : caikai
    修改内容   : 创建文件

******************************************************************************/


#ifndef __IMSNIC_H__
#define __IMSNIC_H__

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
  1 其他头文件包含
*****************************************************************************/
#include  "ImsNicInterface.h"

#if( FEATURE_ON == FEATURE_IMS )
#include  "vos.h"
#include  "PsTypeDef.h"
#include  "PsCommonDef.h"


#if (VOS_OS_VER == VOS_WIN32)
#include  "VxWorksStub.h"
#else
#include  "net/if.h"
#include  "end.h"
#include  "netBufLib.h"
#include  "ipcom_sysvar.h"
#include  "ipcom_vxworks.h"
#endif      /* VOS_OS_VER == VOS_WIN32 */




#pragma pack(4)

/*****************************************************************************
  2 宏定义
*****************************************************************************/
#define UEPS_PID_IMSNIC                    UEPS_PID_IPS_CCORE


#define IMS_NIC_RAB_ID_MAX_NUM             (11)                         /* RAB的最大个数值 */

#define IMS_NIC_RAB_ID_OFFSET              (5)

#define IMS_NIC_RAB_ID_MIN                 (5)                          /* Rab Id的最小值 */
#define IMS_NIC_RAB_ID_MAX                 (15)                         /* Rab Id的最大值 */

#define IMS_NIC_RAB_ID_INVALID             (0xFF)

#define IMS_NIC_ETH_HDR_SIZE               (14)
#define IMS_NIC_ETHER_ADDR_LEN             (6)

#define IMS_NIC_IPV4_VERSION               (4)                          /* IP头部中IP V4版本号 */
#define IMS_NIC_IPV6_VERSION               (6)                          /* IP头部中IP V6版本号 */
#define IMS_NIC_IP_VERSION_POS             (4)                          /* IP数据包的协议版本字段偏移 */
#define IMS_NIC_IP_VERSION_MASK            (0x0F)                       /* IP数据包的协议版本字段掩码 */

#define IMS_NIC_MIN_IPPKT_LEN              (20)                         /* 最小IP包长度 */
#define IMS_NIC_MAX_IPPKT_LEN              (1500)                       /* IMS虚拟网卡限定最大IP包长度 */

#define IMS_NIC_IPPKT_OFFSET               (4)                          /* IP包在上层传下的数据中的偏移 */

#define IMS_NIC_ETHERTYPE_IPV4             (0x0800)                     /* 主机序形式，表示IPv4包类型  */
#define IMS_NIC_ETHERTYPE_IPV6             (0x86DD)                     /* 主机序形式，表示IPv6包类型  */
#define IMS_NIC_ETHERTYPE_ARP              (0x0806)                     /* 主机序形式，表示ARP包类型  */

/* 设备缓存管理设置 */
#define IMS_NIC_CLUSTER_NUM                (128)                        /* cluster的个数 */
#define IMS_NIC_CLUSTER_SIZE               (1516)                       /* 每个cluster的大小 */

#define IMS_NIC_CLBLK_NUM                  (IMS_NIC_CLUSTER_NUM)        /* clBlk控制块的个数 */
#define IMS_NIC_MBLK_NUM                   (8 * IMS_NIC_CLBLK_NUM)      /* MBlock控制块的个数 */

#define IMS_NIC_CMD_LEN                    (200)

#define IMS_NIC_IP_ADDR_MAX_LENGTH         (16)                         /* IP地址的最大长度，取IPV6的长度 */

#define IMS_NIC_UL_SEND_BUFF_SIZE          (1520)                       /* IMS虚拟网卡上行发送缓存大小 */

#define IMS_NIC_SPEED_100M                 (100000000)                  /* 100Mbs */

#define IMS_NIC_IPV6_ADDR_BIT_LEN          (128)
#define IMS_NIC_IPV6_ADDR_STR_LEN          (39)
#define IMS_NIC_IPV6_STR_RFC2373_TOKENS    (8)
#define IMS_NIC_IPV6_STR_DELIMITER         ':'                          /* RFC2373使用的IPV6文本表达方式使用的分隔符 */

#define IMS_NIC_MODEM_ID_LEN               (2)                          /* ModemId占用2个字节*/
#define IMS_NIC_MODEM_ID_OFFSET            (4)                          /* 填写可维可测时使用，保持字节对齐 */

#define IMS_NIC_LOG_RECORD_MAX_NUM         (10)
#define IMS_NIC_LOG_PARAM_MAX_NUM          (4)

#define IMS_NIC_TYPE_HEX                   (16)
#define IMS_NIC_TYPE_DEC                   (10)

#define IMS_NIC_MTU_DEFAULT_VALUE          (1280)                       /* MTU默认值 */

#define IMS_NIC_IS_RAB_VALID(rabId) \
        ((rabId >= IMS_NIC_RAB_ID_MIN) && (rabId <= IMS_NIC_RAB_ID_MAX))

#define IMS_NIC_END_HADDR(pEnd)     \
        ((pEnd)->mib2Tbl.ifPhysAddress.phyAddress)

#define IMS_NIC_END_HADDR_LEN(pEnd) \
        ((pEnd)->mib2Tbl.ifPhysAddress.addrLength)

/* 统计操作 */
#define IMS_NIC_STATS_INC(type, n, modem)  \
        (g_astImsNicEntity[modem].stImsNicStatsInfo.aulStats[type] += n)

#define IMS_INC_STATS_SHOW(type, modem) \
        vos_printf("%s      %d \n", #type, g_astImsNicEntity[modem].stImsNicStatsInfo.aulStats[type])

/* 获取IMS虚拟网卡实体 */
#define IMS_NIC_GET_ENTITY_STRU(modem)              (&g_astImsNicEntity[modem])
/* 获取虚拟网卡名 */
#define IMS_NIC_GET_NIC_NAME                        (g_stImsNicManageTbl.pcImsNicName)
/* 获取目的MAC地址 */
#define IMS_NIC_GET_DST_MAC_ADDR(modem) \
        (g_stImsNicManageTbl.astImsNicElementTbl[modem].stIpv4Ethhead.aucEtherDhost)

#define IMS_NIC_IPCOM_DNSS_CONFIG(indx, ipAddrStr) \
        IMS_NIC_MNTN_TRACE2(ID_IMS_NIC_MNTN_TRACE_DNS_CFG, ipAddrStr, sizeof(ipAddrStr), indx, (VOS_UINT32)ipcom_sysvar_set(g_stImsNicManageTbl.pcDnsSSysVarName[indx], (VOS_CHAR *)ipAddrStr, IPCOM_SYSVAR_FLAG_OVERWRITE))

#define IMS_NIC_IPCOM_DNSS_DEL(indx) \
        IMS_NIC_MNTN_TRACE2(ID_IMS_NIC_MNTN_TRACE_DNS_DEL, "", sizeof(""), indx, (VOS_UINT32)ipcom_sysvar_set(g_stImsNicManageTbl.pcDnsSSysVarName[indx], (VOS_CHAR *)"", IPCOM_SYSVAR_FLAG_OVERWRITE))

/* 获取IMS虚拟网卡设备驱动表 */
#define IMS_NIC_GET_END_FUNC_TABLE_PTR              (&(g_stImsNicManageTbl.stImsNicEndFuncTable))
/* 获取IMS虚拟网卡基本信息表 */
#define IMS_NIC_GET_ELEMENT_TABLE(modem)            (&(g_stImsNicManageTbl.astImsNicElementTbl[modem]))

/* 获取IMS虚拟网卡设备结构 */
#define IMS_NIC_GET_DEV_STRU_PTR(modem)             (&g_astImsNicDev[modem])

#define IMS_NIC_GET_MTU                             (g_stImsNicCtx.ulMtu)               /* 获取IMS虚拟网卡MTU值 */
#define IMS_NIC_SET_MTU(n)                          (g_stImsNicCtx.ulMtu = (n))         /* 设置IMS虚拟网卡MTU值 */

#define IMS_NIC_GET_SAVED_UL_DATA_FUNC(modem)       (g_stImsNicCtx.pTmpSendFunc[modem])
#define IMS_NIC_SAVE_UL_DATA_FUNC(modem, pFunc)     (g_stImsNicCtx.pTmpSendFunc[modem]  = (pFunc))

#define IMS_NIC_IP4_TO_STR(str, ipArray)  \
        VOS_sprintf((VOS_CHAR *)str, "%d.%d.%d.%d", ipArray[0], ipArray[1], ipArray[2], ipArray[3])

#define IMS_NIC_RUN_CMD(cmd) \
        IMS_NIC_MNTN_TRACE1(ID_IMS_NIC_MNTN_TRACE_RUN_CMD, cmd, sizeof(cmd), (VOS_UINT32)ipcom_run_cmd((VOS_CHAR *)cmd)) \

#define IMS_NIC_SET_CMD_END(cmd, indx) \
        PS_MEM_SET(&cmd[indx], 0, sizeof(cmd) - indx); \
        (cmd[indx] = '\0')

#define IMS_NIC_ADD_IPV4_CMD(cmd, modem, nicName, ipv4Array) \
        VOS_sprintf((VOS_CHAR *)cmd, "ifconfig %s%d inet add %d.%d.%d.%d up", nicName, modem, ipv4Array[0], ipv4Array[1], ipv4Array[2], ipv4Array[3])

#define IMS_NIC_DEL_IPV4_CMD(cmd, modem, nicName, ipv4Array) \
        VOS_sprintf((VOS_CHAR *)cmd, "ifconfig %s%d inet del %d.%d.%d.%d", nicName, modem, ipv4Array[0], ipv4Array[1], ipv4Array[2], ipv4Array[3])

#define IMS_NIC_IPV4_ROUTE_CMD(cmd, modem, nicName) \
        VOS_sprintf((VOS_CHAR *)cmd, "route add -n -dev %s%d -inet default", nicName, modem)

#define IMS_NIC_IPV4_ROUTE_DEL_CMD(cmd, modem, nicName) \
        VOS_sprintf((VOS_CHAR *)cmd, "route delete -dev %s%d -inet default", nicName, modem)


#define IMS_NIC_STATIC_ARP_CMD(cmd, ipv4Array, macArray) \
        VOS_sprintf((VOS_CHAR *)cmd, "arp -s %d.%d.%d.%d %x:%x:%x:%x:%x:%x", ipv4Array[0], ipv4Array[1], ipv4Array[2], ipv4Array[3], macArray[0], macArray[1], macArray[2], macArray[3], macArray[4], macArray[5])

#define IMS_NIC_DOWN_NIC_CMD(cmd, modem, nicName) \
        VOS_sprintf((VOS_CHAR *)cmd, "ifconfig %s%d down", nicName, modem)

#define IMS_NIC_DEL_ARP_CMD(cmd, ipv4Array) \
        VOS_sprintf((VOS_CHAR *)cmd, "arp -d %d.%d.%d.%d", ipv4Array[0], ipv4Array[1], ipv4Array[2], ipv4Array[3])

#define IMS_NIC_ADD_IPV6_CMD(cmd, modem, nicName, ipv6AddrStr, prefixLen) \
        VOS_sprintf((VOS_CHAR *)cmd, "ifconfig %s%d inet6 add %s prefixlen %d up", nicName, modem, ipv6AddrStr, prefixLen)

#define IMS_NIC_DEL_IPV6_CMD(cmd, modem, nicName, ipv6AddrStr) \
        VOS_sprintf((VOS_CHAR *)cmd, "ifconfig %s%d inet6 del %s ", nicName, modem, ipv6AddrStr)

#define IMS_NIC_IPV6_ROUTE_CMD(cmd, modem, nicName) \
        VOS_sprintf((VOS_CHAR *)cmd, "route add -n -dev %s%d -inet6 default", nicName, modem)

#define IMS_NIC_SET_PORT_MIN_CMD(cmd, minPort) \
        VOS_sprintf((VOS_CHAR *)cmd, "sysvar set -o ipnet.sock.AnonPortMin %d", minPort)

#define IMS_NIC_SET_PORT_MAX_CMD(cmd, maxPort) \
        VOS_sprintf((VOS_CHAR *)cmd, "sysvar set -o ipnet.sock.AnonPortMax %d", maxPort)

#define IMS_NIC_IPV6_ROUTE_DEL_CMD(cmd, modem, nicName) \
        VOS_sprintf((VOS_CHAR *)cmd, "route delete -dev %s%d -inet6 default", nicName, modem)

#define IMS_NIC_STATIC_NEIGHBOR(cmd, modem, nicName, macArray) \
        VOS_sprintf((VOS_CHAR *)cmd, "ndp -s fe80::8%d %s%d %x:%x:%x:%x:%x:%x", modem, nicName, modem, macArray[0], macArray[1], macArray[2], macArray[3], macArray[4], macArray[5])

#define IMS_NIC_DEL_NEIGHBOR(cmd, modem, nicName) \
        VOS_sprintf((VOS_CHAR *)cmd, "ndp -d fe80::8%d %s%d ", modem, nicName, modem)

#define IMS_NIC_IPV4_DNS_CONFIG(ipAddr, pstDnsInfo, pstPdnInfo) \
        IMS_NIC_DnsConfig(ipAddr, IMS_NIC_IPV4_ADDR_LENGTH, pstDnsInfo, pstPdnInfo)

#define IMS_NIC_IPV6_DNS_CONFIG(ipAddr, pstDnsInfo, pstPdnInfo) \
        IMS_NIC_DnsConfig(ipAddr, IMS_NIC_IPV6_ADDR_LENGTH, pstDnsInfo, pstPdnInfo)

#define IMS_NIC_SET_MTU_CMD(cmd, modem, nicName, value) \
        VOS_sprintf((VOS_CHAR *)cmd, "ifconfig %s%d mtu %d", nicName, modem, value)


#define IMS_NIC_INFO_LOG(ModuleId, str)
#define IMS_NIC_INFO_LOG1(ModuleId, str, para1)
#define IMS_NIC_INFO_LOG2(ModuleId, str, para1, para2)
#define IMS_NIC_INFO_LOG3(ModuleId, str, para1, para2, para3)
#define IMS_NIC_INFO_LOG4(ModuleId, str, para1, para2, para3, para4)

#ifdef __UT_CENTER__
#define IMS_NIC_WARNING_LOG(ModuleId, str) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, 0, 0, 0, 0); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)

#define IMS_NIC_WARNING_LOG1(ModuleId, str, para1) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, (VOS_INT32)para1, 0, 0, 0); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)

#define IMS_NIC_WARNING_LOG2(ModuleId, str, para1, para2) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, (VOS_INT32)para1, (VOS_INT32)para2, 0, 0); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)

#define IMS_NIC_WARNING_LOG3(ModuleId, str, para1, para2, para3) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, (VOS_INT32)para1, (VOS_INT32)para2, (VOS_INT32)para3, 0); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)

#define IMS_NIC_WARNING_LOG4(ModuleId, str, para1, para2, para3, para4) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_WARNING, THIS_FILE_ID, __LINE__, (VOS_INT32)para1, (VOS_INT32)para2, (VOS_INT32)para3, (VOS_INT32)para4); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)

#define IMS_NIC_ERROR_LOG(ModuleId, str) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_ERROR, THIS_FILE_ID, __LINE__, 0, 0, 0, 0); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)

#define IMS_NIC_ERROR_LOG1(ModuleId, str, para1) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_ERROR, THIS_FILE_ID, __LINE__, (VOS_INT32)para1, 0, 0, 0); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)

#define IMS_NIC_ERROR_LOG2(ModuleId, str, para1, para2) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_ERROR, THIS_FILE_ID, __LINE__, (VOS_INT32)para1, (VOS_INT32)para2, 0, 0); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)

#define IMS_NIC_ERROR_LOG3(ModuleId, str, para1, para2, para3) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_ERROR, THIS_FILE_ID, __LINE__, (VOS_INT32)para1, (VOS_INT32)para2, (VOS_INT32)para3, 0); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)

#define IMS_NIC_ERROR_LOG4(ModuleId, str, para1, para2, para3, para4) \
        IMS_NIC_MNTN_LogSave(&g_stImsNicCtx.stLogEnt, ModuleId, LOG_LEVEL_ERROR, THIS_FILE_ID, __LINE__, (VOS_INT32)para1, (VOS_INT32)para2, (VOS_INT32)para3, (VOS_INT32)para4); \
        IMS_NIC_MNTN_LogOutput(&g_stImsNicCtx.stLogEnt, ModuleId)
#else
#define IMS_NIC_WARNING_LOG(ModuleId, str)                                  vos_printf(" %s, %d, %s\r\n ", __FILE__, __LINE__,str)
#define IMS_NIC_WARNING_LOG1(ModuleId, str, para1)                          vos_printf (" %s, %d, %s, %d, \r\n ",  __FILE__, __LINE__, str, para1)
#define IMS_NIC_WARNING_LOG2(ModuleId, str, para1, para2)                   vos_printf (" %s, %d, %s, %d, %d \r\n ",  __FILE__, __LINE__, str, para1, para2)
#define IMS_NIC_WARNING_LOG3(ModuleId, str, para1, para2, para3)            vos_printf (" %s, %d, %s, %d, %d, %d\r\n ",  __FILE__, __LINE__, str, para1, para2, para3)
#define IMS_NIC_WARNING_LOG4(ModuleId, str, para1, para2, para3, para4)     vos_printf (" %s, %d, %s, %d, %d, %d, %d\r\n ",  __FILE__, __LINE__, str, para1, para2, para3, para4)

#define IMS_NIC_ERROR_LOG(ModuleId, str)                                    vos_printf(" %s, %d, %s\r\n ", __FILE__, __LINE__,str)
#define IMS_NIC_ERROR_LOG1(ModuleId, str, para1)                            vos_printf (" %s, %d, %s, %d, \r\n ",  __FILE__, __LINE__, str, para1)
#define IMS_NIC_ERROR_LOG2(ModuleId, str, para1, para2)                     vos_printf (" %s, %d, %s, %d, %d \r\n ",  __FILE__, __LINE__, str, para1, para2)
#define IMS_NIC_ERROR_LOG3(ModuleId, str, para1, para2, para3)              vos_printf (" %s, %d, %s, %d, %d, %d\r\n ",  __FILE__, __LINE__, str, para1, para2, para3)
#define IMS_NIC_ERROR_LOG4(ModuleId, str, para1, para2, para3, para4)       vos_printf (" %s, %d, %s, %d, %d, %d, %d\r\n ",  __FILE__, __LINE__, str, para1, para2, para3, para4)
#endif

/* IMS虚拟网卡初始化期间的错误打印，直接打印到串口 */

#define IMS_NIC_INIT_LOG_PRINT(ModuleId, String)                              vos_printf(" %s, %d, %s\r\n ", __FILE__, __LINE__,String)
#define IMS_NIC_INIT_LOG_PRINT1(ModuleId, String,Para1)                       vos_printf (" %s, %d, %s, %d, \r\n ",  __FILE__, __LINE__, String, Para1)
#define IMS_NIC_INIT_LOG_PRINT2(ModuleId, String,Para1,Para2)                 vos_printf (" %s, %d, %s, %d, %d \r\n ",  __FILE__, __LINE__, String, Para1, Para2)
#define IMS_NIC_INIT_LOG_PRINT3(ModuleId, String,Para1,Para2,Para3)           vos_printf (" %s, %d, %s, %d, %d, %d\r\n ",  __FILE__, __LINE__, String, Para1, Para2, Para3)
#define IMS_NIC_INIT_LOG_PRINT4(ModuleId, String,Para1,Para2,Para3,Para4)     vos_printf (" %s, %d, %s, %d, %d, %d, %d\r\n ",  __FILE__, __LINE__, String, Para1, Para2, Para3, Para4)

/* 转定义LogMsg函数，在阻塞性操作中使用 */
#define IMS_NIC_ERROR_LOG_MSG(ModuleId, fmt)                                  logMsg((fmt), 0, 0, 0, 0, 0, 0)
#define IMS_NIC_ERROR_LOG_MSG1(ModuleId, fmt,Para1)                           logMsg((fmt), (Para1), 0, 0, 0, 0, 0)
#define IMS_NIC_ERROR_LOG_MSG2(ModuleId, fmt,Para1,Para2)                     logMsg((fmt), (Para1), (Para2), 0, 0, 0, 0)
#define IMS_NIC_ERROR_LOG_MSG3(ModuleId, fmt,Para1,Para2,Para3)               logMsg((fmt), (Para1), (Para2), (Para3), 0, 0, 0)
#define IMS_NIC_ERROR_LOG_MSG4(ModuleId, fmt,Para1,Para2,Para3,Para4)         logMsg((fmt), (Para1), (Para2), (Para3), (Para4), 0, 0)

#define IMS_NIC_MNTN_TRACE(msgId, pData, ulLen)   \
        IMS_NIC_MntnTrace(msgId, (VOS_UINT8 *)pData, ulLen, 0, 0)

#define IMS_NIC_MNTN_TRACE1(msgId, pData, ulLen, ulPara1)   \
        IMS_NIC_MntnTrace(msgId, (VOS_UINT8 *)pData, ulLen, ulPara1, 0)

#define IMS_NIC_MNTN_TRACE2(msgId, pData, ulLen, ulPara1, ulPara2)   \
        IMS_NIC_MntnTrace(msgId, (VOS_UINT8 *)pData, ulLen, ulPara1, ulPara2)


#define IMS_NIC_MNTN_TRACE_NO_DATA(msgId, ulPara1, ulPara2)   \
        IMS_NIC_MntnTrace(msgId, VOS_NULL_PTR, 0, ulPara1, ulPara2)

/* ----------------VxWorks 系统函数封装--------------------------- */
#if (VOS_OS_VER == VOS_WIN32)
#define IMS_NIC_MALLOC(size)                    VOS_MemAlloc(UEPS_PID_IMSNIC, STATIC_MEM_PT, size)
#define IMS_NIC_FREE(pAddr)                     VOS_MemFree(UEPS_PID_IMSNIC, pAddr)
#else
#define IMS_NIC_MALLOC(size)                    malloc(size)
#define IMS_NIC_FREE(pAddr)                     free(pAddr)
#endif

#define IMS_NIC_MEMALIGN(alignment, size)       memalign(alignment, size)

#define IMS_NETPOOL_INIT(pNetPool, pMclBlkConfig, pClDescTbl) \
        netPoolInit(pNetPool, pMclBlkConfig, pClDescTbl, 1, VOS_NULL_PTR)

#define IMS_NIC_POOL_ID_GET(poolId, bufSize)    clPoolIdGet(poolId, bufSize, VOS_FALSE)

#define IMS_NIC_FIND_BY_NAME(baseName, unit)    endFindByName(baseName, unit)

#define IMS_NIC_CLUSTER_GET(pNetPool, pClPool)  netClusterGet(pNetPool, pClPool)

#define IMS_NIC_CLUSTER_FREE(pNetPool, pClBuf)  netClFree(pNetPool, pClBuf)

#define IMS_NIC_CLBLK_GET(pNetPool)             netClBlkGet(pNetPool, M_DONTWAIT)

#define IMS_NIC_CLBLK_FREE(pNetPool, pClBlk)    netClBlkFree(pNetPool, pClBlk)

#define IMS_NIC_MBLK_GET(poolId)                mBlkGet(poolId, M_DONTWAIT, MT_DATA)

#define IMS_NIC_CLBLK_JOIN(pClBlk, pClBuf, size) \
        netClBlkJoin(pClBlk, pClBuf, size, VOS_NULL_PTR, 0, 0, 0)

#define IMS_NIC_MBLK_JOIN(pMblk, pClBlk)        netMblkClJoin(pMblk, pClBlk)

#define IMS_NIC_MUX_DEV_LOAD(unit, endLoad) \
        muxDevLoad(unit, endLoad, "", 1, VOS_NULL_PTR)

#define IMS_NIC_MUX_START(pCookie)              muxDevStart(pCookie)

#define IMS_NIC_MUX_DEV_UNLOAD(pName, unit)     muxDevUnload(pName, unit)

#define IMS_NIC_NETPOOL_DEL(pNetPool)           netPoolDelete(pNetPool)

#define IMS_NIC_MBLK_CHAIN_FREE(pMblk)          netMblkClChainFree(pMblk)

#define IMS_NIC_DRV_ETH_INIT(name, unit)        ipcom_drv_eth_init(name, unit, IP_NULL)


/*****************************************************************************
  3 枚举定义
*****************************************************************************/
enum IMS_NIC_MODEM_ID_ENUM
{
    IMS_NIC_MODEM_ID_0 = 0,

    IMS_NIC_MODEM_ID_BUTT       /* 考虑到目前副卡不需要IMS网卡，故暂时设置BUTT值为1*/
};

typedef VOS_UINT16 IMS_NIC_MODEM_ID_ENUM_UINT16;

/* DNS服务器编号 */
enum IMS_NIC_DNS_NUMBER_ENUM
{
     IMS_NIC_DNS_PRIMARY                                   = 0,         /* 主DNS服务器 */
     IMS_NIC_DNS_SECONDARY,                                             /* 第二DNS服务器 */
     IMS_NIC_DNS_TERTIARY,                                              /* 第三DNS服务器 */
     IMS_NIC_DNS_QUATERNARY,                                            /* 第四DNS服务器 */

     IMS_NIC_DNS_BUTT
};
typedef VOS_UINT8 IMS_NIC_DNS_NUMBER_ENUM_UINT8;

/* IMS虚拟网卡统计枚举 */
enum IMS_NIC_STATS_INFO_ENUM
{
    IMS_NIC_STATS_DL_RCV_IPV4_PKT = 0,                                  /* IMS虚拟网卡收到下行IPV4数据的个数 */
    IMS_NIC_STATS_DL_RCV_IPV6_PKT,                                      /* IMS虚拟网卡收到下行IPV6数据的个数 */
    IMS_NIC_STATS_DL_SEND_PKT,                                          /* IMS虚拟网卡发送下行数据的个数 */
    IMS_NIC_STATS_DL_RCV_BYTES,                                         /* 下行收到数据包byte数 */
    IMS_NIC_STATS_DL_RCV_ERR_PKT,                                       /* 收到错误数据包的个数(非ipv4、ipv6包) */
    IMS_NIC_STATS_DL_INPUT_NULL_PTR,                                    /* 输入数据为空的次数 */
    IMS_NIC_STATS_DL_RCV_DATA_LEN_ERR,                                  /* IMS虚拟网卡收到下行数据长度错误的个数 */

    IMS_NIC_STATS_UL_RCV_IPV4_PKT,                                      /* IMS虚拟网卡收到上行IPV4数据的个数 */
    IMS_NIC_STATS_UL_RCV_IPV6_PKT,                                      /* IMS虚拟网卡收到上行IPV6数据的个数 */
    IMS_NIC_STATS_UL_SEND_PKT,                                          /* IMS虚拟网卡上行发送给CDS数据的个数 */
    IMS_NIC_STATS_UL_SEND_BYTES,                                        /* 上行发送的数据包byte数 */
    IMS_NIC_STATS_UL_RCV_ERR_PKT,                                       /* 收到错误数据包的个数(非ipv4、ipv6包) */
    IMS_NIC_STATS_UL_RCV_ARP_PKT,                                       /* 收到ARP包的个数 */
    IMS_NIC_STATS_UL_INPUT_NULL_PTR,                                    /* 输入数据为空的次数 */
    IMS_NIC_STATS_UL_RCV_DATA_LEN_ERR,                                  /* IMS虚拟网卡收到上行数据长度错误的个数 */
    IMS_NIC_STATS_UL_CB_FUN_NOT_REG,                                    /* IMS虚拟网卡上行数据发送函数没有注册次数 */
    IMS_NIC_STATS_UL_CALL_CB_FUN_FAIL,                                  /* 调用CDS注册的回调函数失败次数 */
    IMS_NIC_STATS_UL_PKT_MEM_SEG,                                       /* 数据包内存分段 */

    IMS_NIC_STATS_CFG_DNS_SERVER_FULL,
    IMS_NIC_STATS_CFG_INPUT_NULL_PTR,                                   /* 配置指针为空 */
    IMS_NIC_STATS_CFG_ADD_MODEM_ID_ERR,                                 /* 配置时传入的modem id匈牙利 */
    IMS_NIC_STATS_CFG_DEL_MODEM_ID_ERR,                                 /* 删除配置传入的ModemId错误 */
    IMS_NIC_STATS_CFG_ADD_RAB_ID_ERR,                                   /* 增加配置输入的RABID错误 */
    IMS_NIC_STATS_CFG_DEL_RAB_ID_ERR,                                   /* 删除输入的RABID错误 */
    IMS_NIC_STATS_GET_CLUSTER_FAIL,                                     /* 获取Cluster失败次数 */
    IMS_NIC_STATS_GET_CLBLK_FAIL,                                       /* 获取clBlk失败 */
    IMS_NIC_STATS_GET_MBLK_FAIL,
    IMS_NIC_STATS_MBLK_COPY_ERR,                                        /* 拷贝Mblk内存失败 */
    IMS_NIC_STATS_NOT_INITED_ERR,                                       /* 虚拟网卡没有初始化 */
    IMS_NIC_STATS_SYS_CALL_FAIL,                                        /* 系统调用失败 */
    IMS_NIC_STATS_FIND_DEV_FAIL,                                        /* 查找设备失败 */
    IMS_NIC_STATS_NO_CFGED_PDN,
    IMS_NIC_STATS_CFG_ADD_NO_PDN,
    IMS_NIC_STATS_CFG_DEL_NO_PDN,
    IMS_NIC_STATS_MODEM_ID_ERR_OTHER,
    IMS_NIC_STATS_MODEM_ID_ERR_IN_REG_CB,
    IMS_NIC_STATS_MODE_ID_ERR_IN_DL_PROC,
    IMS_NIC_STATS_MODE_ID_ERR_IN_UL_PROC,
    IMS_NIC_STATS_MTU_INVALID,
    IMS_NIC_STATS_INVALID_IPV6_PREFIX_LEN,

    IMS_NIC_STATS_BUTT,
};

typedef VOS_UINT32 IMS_NIC_STATS_INFO_ENUM_UINT32;

enum IMS_NIC_MNTN_TRACE_ID_ENUM
{
    ID_IMS_NIC_MNTN_TRACE_RCV_CONFIG_INFO       = 0xD5F1,
    ID_IMS_NIC_MNTN_TRACE_CONFIG_PROC_RSLT      = 0xD5F2,
    ID_IMS_NIC_MNTN_TRACE_RCV_CONFIG_DEL        = 0xD5F3,
    ID_IMS_NIC_MNTN_TRACE_RCV_CONFIG_DEL_RSLT   = 0xD5F4,
    ID_IMS_NIC_MNTN_TRACE_RUN_CMD               = 0xD5F5,
    ID_IMS_NIC_MNTN_TRACE_DNS_CFG               = 0xD5F6,
    ID_IMS_NIC_MNTN_TRACE_DNS_DEL               = 0xD5F7,
    ID_IMS_NIC_MNTN_TRACE_LOG_MSG               = 0xD5F8,

    ID_IMS_NIC_MNTN_TRACE_BUTT
};

enum IMS_NIC_DNS_STATE_ENUM
{
    IMS_NIC_DNS_CFGED                           = 0,
    IMS_NIC_DNS_NOT_CFGED,
    IMS_NIC_DNS_STATE_BUTT
};
/*****************************************************************************
  4 全局变量声明
*****************************************************************************/

/*****************************************************************************
  5 消息头定义
*****************************************************************************/


/*****************************************************************************
  6 消息定义
*****************************************************************************/


/*****************************************************************************
  7 STRUCT定义
*****************************************************************************/
/*****************************************************************************
 结构名    : IMS_NIC_SINGLE_PDN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 某个承载上IMS虚拟网卡PDN信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          bitOpIpv4PdnInfo            : 1;                        /* 如果为1则代表aucIpV4Addr有效 */
    VOS_UINT32                          bitOpIpv6PdnInfo            : 1;                        /* 如果为1则代表aucIpV6Addr有效 */
    VOS_UINT32                          bitOpIpv4DnsPrimCfged       : 1;                     /* IMS NIC内部填写，记录aucDnsPrimAddr是否配置成功 */
    VOS_UINT32                          bitOpIpv4DnsSecCfged        : 1;                     /* IMS NIC内部填写，记录aucDnsSecAddr是否配置成功 */
    VOS_UINT32                          bitOpIpv6DnsPrimCfged       : 1;                     /* IMS NIC内部填写，记录aucDnsPrimAddr是否配置成功 */
    VOS_UINT32                          bitOpIpv6DnsSecCfged        : 1;                     /* IMS NIC内部填写，记录aucDnsSecAddr是否配置成功 */

    VOS_UINT32                          bitOpSpare                  : 26;

    VOS_UINT8                           aucDnsFlag[IMS_NIC_MAX_DNS_SERVER_NUM];         /* 如果为1则代表配置了DNS服务器,分别对应主、第二、第三和第四DNS服务器 */

    IMS_NIC_IPV4_PDN_INFO_STRU          stIpv4PdnInfo;
    IMS_NIC_IPV6_PDN_INFO_STRU          stIpv6PdnInfo;
}IMS_NIC_SINGLE_PDN_INFO_STRU;

/*****************************************************************************
 结构名    : IMS_NIC_SINGLE_DNS_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 单个DNS配置信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_INT8                            cDnsRef;                                   /* DNS服务器的引用计数 */
    VOS_UINT8                           ucIpAddrLen;                                /* 存储的DNS服务器IP地址长度 */
    VOS_UINT8                           aucReserved[2];
    VOS_UINT8                           aucDnsAddr[IMS_NIC_IP_ADDR_MAX_LENGTH];     /* DNS服务器地址 */
}IMS_NIC_SINGLE_DNS_INFO_STRU;

/*****************************************************************************
 结构名    : IMS_NIC_DNS_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : DNS配置信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                           ucDnsServerCnt;                                     /* 当前配置DNS服务器的个数 */
    VOS_UINT8                           aucReserved[3];
    IMS_NIC_SINGLE_DNS_INFO_STRU        astImsNicDnsInfo[IMS_NIC_MAX_DNS_SERVER_NUM];       /* DNS配置信息，分别对应主、第一、第二、第三DNS服务器 */
}IMS_NIC_DNS_INFO_STRU;

/*****************************************************************************
 结构名    : IMS_NIC_PDN_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 所有承载上IMS虚拟网卡PDN信息记录结构
*****************************************************************************/
typedef struct
{
    VOS_INT                             iPdnCfgCnt;
    IMS_NIC_DNS_INFO_STRU               stImsNicDnsInfo;                                /* 当前DNS服务器配置信息 */
    IMS_NIC_SINGLE_PDN_INFO_STRU        astImsNicPdnInfo[IMS_NIC_RAB_ID_MAX_NUM];       /* 各Rab上PDN激活信息 */
}IMS_NIC_PDN_INFO_STRU;

/*****************************************************************************
 结构名    : IMS_NIC_DEV_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 用于控制设备的主数据结构
*****************************************************************************/
typedef struct
{
    END_OBJ                   end;                                              /* END对象，系统通过它来控制设备驱动 */
    VOS_INT                   unit;                                             /* 设备的unit号 */
    M_CL_CONFIG               mclCfg;                                           /* MBlock的配置信息 */
    CL_DESC                   clDesc;                                           /* 网络缓存池配置信息 */
    CL_POOL_ID                pClPoolId;                                        /* 用于获取网络缓存 */
    VOS_UINT8                 aucSendBuf[IMS_NIC_UL_SEND_BUFF_SIZE];            /* 上行发送缓存,1520字节*/
} IMS_NIC_DEV_STRU;

/*****************************************************************************
 结构名    : IMS_NIC_ETH_HEADER_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : 以太网头结构
*****************************************************************************/
typedef struct
{
    VOS_UINT8                 aucEtherDhost[IMS_NIC_ETHER_ADDR_LEN];              /* 目的以太网地址 */
    VOS_UINT8                 aucEtherShost[IMS_NIC_ETHER_ADDR_LEN];              /* 源以太网地址 */
    VOS_UINT16                usEtherType;                                       /* 包类型 */
}IMS_NIC_ETH_HEADER_STRU;

/*****************************************************************************
 结构名    : IMS_NIC_ELEMENT_TAB_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IMS虚拟网卡信息
*****************************************************************************/
typedef struct
{
    IMS_NIC_ETH_HEADER_STRU             stIpv4Ethhead;                          /* IPV4以太网头 */
    IMS_NIC_ETH_HEADER_STRU             stIpv6Ethhead;                          /* IPV6以太网头 */
}IMS_NIC_ELEMENT_TAB_STRU;

/*****************************************************************************
 结构名    : IMS_NIC_STATS_INFO_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IMS虚拟网卡统计信息
*****************************************************************************/
typedef struct
{
    VOS_UINT32              aulStats[IMS_NIC_STATS_BUTT];
}IMS_NIC_STATS_INFO_STRU;

/*****************************************************************************
 结构名    : IMS_NIC_ENTITY_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IMS虚拟网卡实体
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulImsNicInitFlag;                       /* IMS虚拟网卡初始化标记 */
    IMS_NIC_SEND_UL_DATA_FUNC           pUlDataSendFunc;                        /* IMS虚拟网卡上行数据发送函数 */
    IMS_NIC_PDN_INFO_STRU               stImsNicPdnInfo;                        /* IMS虚拟网卡PDN配置信息 */
    IMS_NIC_STATS_INFO_STRU             stImsNicStatsInfo;                      /* IMS虚拟网卡统计信息 */
}IMS_NIC_ENTITY_STRU;

/*****************************************************************************
 结构名    : IMS_NIC_MANAGE_TBL_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IMS虚拟网卡管理结构
*****************************************************************************/
typedef struct
{
    NET_FUNCS                           stImsNicEndFuncTable;                           /* IMS虚拟网卡设备驱动表 */
    VOS_CHAR                           *pcImsNicName;                                   /* 网卡名称 */
    IMS_NIC_ELEMENT_TAB_STRU            astImsNicElementTbl[IMS_NIC_MODEM_ID_BUTT];     /* IMS虚拟网卡基本信息表 */
    VOS_CHAR                           *pcDnsSSysVarName[IMS_NIC_MAX_DNS_SERVER_NUM];   /* DNS系统变量名 */
}IMS_NIC_MANAGE_TBL_STRU;

typedef struct
{
    VOS_MSG_HEADER
    VOS_UINT16 usMsgId;
    VOS_UINT8  aucRsv[2];
    VOS_UINT32 ulPara1;
    VOS_UINT32 ulPara2;
    VOS_UINT8  aucData[4];
}IMS_NIC_MNTN_TRACE_MSG_STRU;

typedef struct
{
    VOS_UINT32                          ulTick;
    VOS_UINT32                          enLevel;
    PS_FILE_ID_DEFINE_ENUM_UINT32       enFile;
    VOS_UINT32                          ulLine;
    VOS_INT32                           alPara[IMS_NIC_LOG_PARAM_MAX_NUM];
}IMS_NIC_LOG_RECORD_STRU;

typedef struct
{
    VOS_UINT32                          ulCnt;                  /* 实际缓存的打印记录数目 */
    IMS_NIC_LOG_RECORD_STRU             astData[IMS_NIC_LOG_RECORD_MAX_NUM];
}IMS_NIC_LOG_ENT_STRU;

typedef struct
{
    VOS_MSG_HEADER                                      /* _H2ASN_Skip */
    VOS_UINT16                          usMsgType;      /* _H2ASN_Skip */
    VOS_UINT16                          usTransId;
    IMS_NIC_LOG_ENT_STRU                stLogMsgCont;   /* LOG_MSG内容 */
}IMS_NIC_TRACE_LOG_MSG_STRU;


/*****************************************************************************
 结构名    : IMS_NIC_CTX_STRU
 协议表格  :
 ASN.1描述 :
 结构说明  : IMS虚拟网卡上下文
*****************************************************************************/
typedef struct
{
    VOS_UINT32                          ulMtu;                                  /* MTU值,默认1280，外部模块可以通过接口修改 */
    VOS_UINT32                          ulInitLogMutexSem;
    VOS_UINT32                          ulSaveLogMutexSem;
    IMS_NIC_SEND_UL_DATA_FUNC           pTmpSendFunc[IMS_NIC_MODEM_ID_BUTT];    /* 记录CDS注册上行回调函数 */
    IMS_NIC_LOG_ENT_STRU                stLogEnt;
}IMS_NIC_CTX_STRU;


/*****************************************************************************
  8 UNION定义
*****************************************************************************/


/*****************************************************************************
  9 OTHERS定义
*****************************************************************************/
extern IMS_NIC_ENTITY_STRU                      g_astImsNicEntity[IMS_NIC_MODEM_ID_BUTT];        /* IMS虚拟网卡实体信息 */
extern const IMS_NIC_MANAGE_TBL_STRU            g_stImsNicManageTbl;
extern IMS_NIC_CTX_STRU                         g_stImsNicCtx;
extern IMS_NIC_DEV_STRU                         g_astImsNicDev[IMS_NIC_MODEM_ID_BUTT];

/*****************************************************************************
  10 函数声明
*****************************************************************************/
extern IP_PUBLIC int    ipcom_run_cmd(char *cmd_str);
extern VOS_UINT32       OM_TraceMsgHook(VOS_VOID *pMsg);
extern VOS_UINT32       OM_GetSlice(VOS_VOID);

VOS_UINT32 IMS_NIC_SetSocketPort(VOS_UINT16 usMinPort, VOS_UINT16 usMaxPort);

VOS_VOID IMS_NIC_SetDefaultRoute(MODEM_ID_ENUM_UINT16 enModemId);

#endif  /*FEATURE_ON == FEATURE_IMS*/

#if (VOS_OS_VER == VOS_WIN32)
#pragma pack()
#else
#pragma pack(0)
#endif




#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

#endif /* end of ImsNicInterface.h */


