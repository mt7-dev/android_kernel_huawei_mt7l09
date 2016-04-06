/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_usb.h
*
*   作    者 :  lvhui
*
*   描    述 :  usb通用对外接口头文件
*
*   修改记录 :  2013年2月2日  v1.00  lvhui创建
*************************************************************************/

#ifndef __DRV_USB_H__
#define __DRV_USB_H__

#include "drv_comm.h"
#include "drv_udi.h"

#ifdef __KERNEL__
#include <linux/skbuff.h>
#endif
/* 设备枚举最大端口个数 */
#define DYNAMIC_PID_MAX_PORT_NUM        17

typedef  enum
{
    FLOW_CON,     /* 流控        */
    PDP_STATUS,   /* PDP激活结果 */
    SIGNAL_LEVEL, /* 信号强度    */
    PKT_STATICS,  /* 流量统计    */
    AT_CMD,       /* AT命令      */
    PS_NOTIFY,    /* Notify命令  */
    RESERVE       /* 保留        */
}NDIS_NOTICE;

typedef unsigned int (*pPCSCFUNC)(unsigned int CmdType, unsigned char *pApdu, unsigned int ApduLen);

/* NV项50091结构，代表设备将要枚举的端口形态 */
typedef struct
{
    unsigned int ulStatus;
    unsigned char aucFirstPortStyle[DYNAMIC_PID_MAX_PORT_NUM];  /* 设备切换前端口形态 */
    unsigned char aucRewindPortStyle[DYNAMIC_PID_MAX_PORT_NUM]; /* 设备切换后端口形态 */
    unsigned char reserved[22];
}DRV_DYNAMIC_PID_TYPE_STRU;

typedef enum
{
    WWAN_WCDMA   = 1,/*WCDMA模式*/
    WWAN_CDMA    = 2  /*CDMA模式*/
}NDIS_WWAN_MODE;

/*管脚信号的定义*/
#define IO_CTRL_FC                      (0x02)
#define IO_CTRL_DSR                     (0x20)
#define IO_CTRL_DTR                     (0x01)
#define IO_CTRL_RFR                     (0x04)
#define IO_CTRL_CTS                     (0x10)
#define IO_CTRL_RI                      (0x40)
#define IO_CTRL_DCD                     (0x80)

typedef int  (*pComRecv)(unsigned char  uPortNo, unsigned char* pData, unsigned short uslength);

typedef int  (*pUartRecv)(unsigned char uPortNo, unsigned char* pData, unsigned short uslength);

typedef int (*pUsbStatus)(unsigned char ucStatus);

typedef int  (*pUsbState)(unsigned char ucStatus);

typedef int  (*pComStatusNotify)(unsigned char uStatus);

/* MODEM 管角信号结构体定义*/
typedef struct tagMODEM_MSC_STRU
{
    BSP_U32 OP_Dtr     :    1;          /*DTR CHANGE FLAG*/
    BSP_U32 OP_Dsr     :    1;          /*DSR CHANGE FLAG*/
    BSP_U32 OP_Cts     :    1;          /*CTSCHANGE FLAG*/
    BSP_U32 OP_Rts     :    1;          /*RTS CHANGE FLAG*/
    BSP_U32 OP_Ri      :    1;          /*RI CHANGE FLAG*/
    BSP_U32 OP_Dcd     :    1;          /*DCD CHANGE FLAG*/
    BSP_U32 OP_Fc      :    1;          /*FC CHANGE FLAG*/
    BSP_U32 OP_Brk     :    1;          /*BRK CHANGE FLAG*/
    BSP_U32 OP_Spare   :    24;         /*reserve*/
    BSP_U8   ucDtr;                     /*DTR  VALUE*/
    BSP_U8   ucDsr;                     /*DSR  VALUE*/
    BSP_U8   ucCts;                     /*DTS VALUE*/
    BSP_U8   ucRts;                     /*RTS  VALUE*/
    BSP_U8   ucRi;                      /*RI VALUE*/
    BSP_U8   ucDcd;                     /*DCD  VALUE*/
    BSP_U8   ucFc;                      /*FC  VALUE*/
    BSP_U8   ucBrk;                     /*BRK  VALUE*/
    BSP_U8   ucBrkLen;                  /*BRKLEN VALUE*/
} MODEM_MSC_STRU, *PMODEM_MSC_STRU,AT_DCE_MSC_STRU;

/*协议栈BULK数据接收回调函数类型定义*/
typedef int (*USB_MODEM_RECV_CALL)
(
    unsigned char ucPortType,          /*端口号*/
    unsigned char ucDlci,                 /*链路号*/
    unsigned char *pData,               /*数据指针*/
    unsigned short usLen                /*数据长度*/
);

/*协议栈消息通道状态变化数据接收回调函数定义*/
typedef int (*USB_MODEM_AT_Sig)
(
    unsigned char ucPortType,          /*端口号*/
    unsigned char ucDlci,                   /*链路号*/
    MODEM_MSC_STRU* data  /*指向MODEM 管角信号结构体的指针*/
);

/*协议栈断链、建链回调函数*/
typedef int (*USB_MODEM_IN_OUT)
(
    unsigned char ucPortType,           /*端口号*/
    unsigned char ucDlci                   /*链路号*/
);

/*协议栈高性能复制函数定义*/
typedef void (*USB_MODEM_COPY)
(
    unsigned char   *pDest,
    unsigned char   *pSrc,
    unsigned int   ulLen
);

typedef struct
{
    unsigned int    GateWayIpAddr;  /* PDP激活获取到的远程网关IP地址      */
    unsigned int    LocalIpAddr;    /* PDP激活获取到的分配到MODEM的IP地址 */
    unsigned int    MaskAddr;       /* 子网掩码 */
    unsigned int    speed;          /* 连接速度，单位bps */
    unsigned char   ActiveSatus;    /* 激活结果，0为成功，其他为失败 */
}NDIS_PDP_STRU,*pNDIS_PDP_STRU;

typedef struct
{
    unsigned int    currentTx;    /* CURRENT_TX */
    unsigned int    currentRx;    /* CURRENT_RX */
    unsigned int    maxTx;        /* MAX_TX */
    unsigned int    maxRx;        /* MAX_RX */
}NDIS_RATE_STRU,*pNDIS_RATE_STRU;

typedef struct
{
    unsigned int     TxOKCount;         /*发送包数*/
    unsigned int     RxOKCount;         /*接收包数*/
    unsigned int     TxErrCount;        /*发送错误*/
    unsigned int     RxErrCount;        /*接收错误*/
    unsigned int     TxOverFlowCount;   /*发送溢出丢包*/
    unsigned int     RxOverFlowCount;   /*接收溢出丢包*/
    unsigned int     MaxTx;         /*发送速率*/
    unsigned int     MaxRx;         /*接收速率*/
} NDIS_PKT_STATISTICS, *pNDIS_PKT_STATISTICS;

typedef struct
{
    unsigned char   *pucAtCmd;          /*指向AT命令的指针*/
    unsigned short   usLen;             /*AT命令的长度*/
}NDIS_AT_CMD_STRU, *pNDIS_AT_CMD_STRU;

/* 信号上报函数使用的结构体*/
typedef struct
{
    NDIS_NOTICE                 type;
    union
    {
        NDIS_PDP_STRU           pdp;
        NDIS_PKT_STATISTICS     pktStatics;
        unsigned char           flowFlag;   /*  1:流控，0：解流控 */
        int                     dbm;
        NDIS_AT_CMD_STRU        atCmd;
    }NDIS_STATUS_DATA;
} NDIS_PRO_STRU, *pNDIS_PRO_STRU;

typedef int (*USB_NDIS_RECV_CALL)
(
    unsigned char *pData, /* 接收缓冲区指针,内容为IP报文 */
    unsigned short usLen  /* 数据长度 */
);

typedef int (*USB_NDIS_BRK)
(
    void
);

typedef int (*USB_NDIS_EST)
(
    unsigned char *pData /* 接收缓冲区指针，内容为连接配置管理参数 */
);

typedef int (*USB_NDIS_PKT_STATISTICS_PERIOD)
(
    unsigned int period /*单位S */
);

typedef int (*USB_NDIS_GET_RATE)
(
    NDIS_RATE_STRU  *pRate
);

typedef int (*USB_NDIS_AT_CMD_RECV)
(
    unsigned char *pBuff,   /*指向AT命令指针*/
    unsigned short usLen    /*AT命令长度*/
);

typedef unsigned int (*USB_NDIS_GET_WWAN_MODE)  /*获取当前网络模式cdma/wcdma*/
(
    void
);

typedef unsigned int (*USB_NET_DEV_SET_MAX_TX_PACKET_NUMBER)
(
    unsigned int ulNumber
);

typedef unsigned int (*MNTN_ERRLOGREGFUN)(char * cFileName,unsigned int ulFileId, unsigned int ulLine,
                unsigned int ulErrNo, void * pBuf, unsigned int ulLen);

/*****************************************************************************
 函 数 名  : BSP_USB_SetPid
 功能描述  :
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern int BSP_USB_SetPid(unsigned char u2diagValue);
#define DRV_SET_PID(u2diagValue)    BSP_USB_SetPid(u2diagValue)

/*****************************************************************************
函数名：   BSP_USB_PortTypeQuery
功能描述:  查询当前的设备枚举的端口形态值
输入参数： stDynamicPidType  端口形态
输出参数： stDynamicPidType  端口形态
返回值：   0:    查询成功
           其他：查询失败
*****************************************************************************/
unsigned int BSP_USB_PortTypeQuery(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType);
#define DRV_SET_PORT_QUIRY(a)   BSP_USB_PortTypeQuery(a)

/*****************************************************************************
函数名：   BSP_USB_PortTypeValidCheck
功能描述:  提供给上层查询设备端口形态配置合法性接口
           1、端口为已支持类型，2、包含PCUI口，3、无重复端口，4、端点数不超过16，
           5、第一个设备不为MASS类
输入参数： pucPortType  端口形态配置
           ulPortNum    端口形态个数
返回值：   0:    端口形态合法
           其他：端口形态非法
*****************************************************************************/
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned int ulPortNum);
#define DRV_USB_PORT_TYPE_VALID_CHECK(pucPortType, ulPortNum)  \
                    BSP_USB_PortTypeValidCheck(pucPortType, ulPortNum)

/*****************************************************************************
函数名：   BSP_USB_GetAvailabePortType
功能描述:  提供给上层查询当前设备支持端口形态列表接口
输入参数： ulPortMax    协议栈支持最大端口形态个数
输出参数:  pucPortType  支持的端口形态列表
           pulPortNum   支持的端口形态个数
返回值：   0:    获取端口形态列表成功
           其他：获取端口形态列表失败
*****************************************************************************/
unsigned int BSP_USB_GetAvailabePortType(unsigned char *pucPortType,
                            unsigned int *pulPortNum, unsigned int ulPortMax);
#define DRV_USB_GET_AVAILABLE_PORT_TYPE(pucPortType, pulPortNum, ulPortMax)  \
                BSP_USB_GetAvailabePortType(pucPortType, pulPortNum, ulPortMax)

/*****************************************************************************
 函 数 名  : BSP_USB_ATProcessRewind2Cmd
 功能描述  : rewind2 CMD 处理。
 输入参数  : pData：数据。
 输出参数  : 无
 返 回 值  : false(0):处理失败
             tool(1):处理成功
*****************************************************************************/
extern int BSP_USB_ATProcessRewind2Cmd(unsigned char *pData);
#define DRV_PROCESS_REWIND2_CMD(pData)    BSP_USB_ATProcessRewind2Cmd(pData)

/*****************************************************************************
 函 数 名  : BSP_USB_GetDiagModeValue
 功能描述  : 获得设备类型。
 输入参数  : 无。
 输出参数  : ucDialmode:  0 - 使用Modem拨号; 1 - 使用NDIS拨号; 2 - Modem和NDIS共存
              ucCdcSpec:   0 - Modem/NDIS都符合CDC规范; 1 - Modem符合CDC规范;
                           2 - NDIS符合CDC规范;         3 - Modem/NDIS都符合CDC规范
 返 回 值  : VOS_OK/VOS_ERR
*****************************************************************************/
extern BSP_S32 BSP_USB_GetDiagModeValue(unsigned char *pucDialmode,
                             unsigned char *pucCdcSpec);
#define DRV_GET_DIAG_MODE_VALUE(pucDialmode, pucCdcSpec)    \
                             BSP_USB_GetDiagModeValue(pucDialmode, pucCdcSpec)

/*****************************************************************************
 函 数 名  : BSP_USB_GetPortMode
 功能描述  : 获取端口形态模式，网关对接需求，打桩。
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 无。
*****************************************************************************/
extern unsigned char BSP_USB_GetPortMode(char*PsBuffer, unsigned int*Length );
#define DRV_GET_PORT_MODE(PsBuffer, Length)    BSP_USB_GetPortMode(PsBuffer,Length)

/*****************************************************************************
 函 数 名  : BSP_USB_GetU2diagDefaultValue
 功能描述  : 获得端口默认u2diag值
 输入参数  : 无。
 输出参数  : 无。
 返回值：   u2diag值

*****************************************************************************/
extern BSP_U32 BSP_USB_GetU2diagDefaultValue(void);
#define DRV_GET_U2DIAG_DEFVALUE()    BSP_USB_GetU2diagDefaultValue()


#ifdef __VXWORKS__
static INLINE void MNTN_ERRLOG_REG_FUNC(MNTN_ERRLOGREGFUN pRegFunc)
{
    return ;
}
#else
extern void MNTN_ERRLOG_REG_FUNC(MNTN_ERRLOGREGFUN pRegFunc);
#endif

/*****************************************************************************
 函 数 名  : BSP_USB_UdiagValueCheck
 功能描述  : 本接口用于检查NV项中USB形态值的合法性.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：OK；
            -1：ERROR
*****************************************************************************/
extern int BSP_USB_UdiagValueCheck(unsigned int DiagValue);
#define DRV_UDIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)
#define DRV_U2DIAG_VALUE_CHECK(DiagValue)     BSP_USB_UdiagValueCheck(DiagValue)

/*****************************************************************************
 函 数 名  : BSP_USB_GetLinuxSysType
 功能描述  : 本接口用于检查PC侧是否为Linux，以规避Linux后台二次拨号失败的问题.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：Linux；
            -1：非Linux。
*****************************************************************************/
extern int BSP_USB_GetLinuxSysType(void) ;
#define DRV_GET_LINUXSYSTYPE()    BSP_USB_GetLinuxSysType()

/********************************************************
函数说明： 返回当前设备列表中支持(sel=1)或者不支持(sel=0)PCSC的设备形态值
函数功能:
输入参数：sel
          0: 通过参数dev_type返回当前不带PCSC是设备形态值
          1：通过参数dev_type返回当前带PCSC是设备形态值
输出参数：dev_type 写入需要的设备形态值，如果没有则不写入值。
          NV中存储的设备形态值
输出参数：pulDevType 与ulCurDevType对应的设备形态值，如果没有返回值1。
返回值：
          0：查询到相应的设备形态值；
          1：没有查询到响应的设备形态值。
********************************************************/
int BSP_USB_PcscInfoSet(unsigned int  ulSel, unsigned int  ulCurDevType, unsigned int *pulDevType);
#define DRV_USB_PCSC_INFO_SET(ulSel, ulCurDevType, pulDevType)    BSP_USB_PcscInfoSet(ulSel, ulCurDevType, pulDevType)

/*TCP/IP协议栈可维可测捕获的消息标识*/
enum IPS_MNTN_TRACE_IP_MSG_TYPE_ENUM
{
    /* IP 数据包可维可测上报 */
    ID_IPS_TRACE_IP_ADS_UL                  = 0xD030,
    ID_IPS_TRACE_IP_ADS_DL                  = 0xD031,
    ID_IPS_TRACE_IP_USB_UL                  = 0xD032,
    ID_IPS_TRACE_IP_USB_DL                  = 0xD033,

    ID_IPS_TRACE_IP_MSG_TYPE_BUTT
};

/********************************************************
函数说明：协议栈注册USB将SKB关键信息上报到SDT回调函数
函数功能:
输入参数：pFunc: SKB关键信息上报到SDT回调函数指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
#ifdef __KERNEL__
typedef void (*USB_IPS_MNTN_TRACE_CB_T)(struct sk_buff *skb,unsigned short usType);
extern unsigned int BSP_USB_RegIpsTraceCB(USB_IPS_MNTN_TRACE_CB_T pFunc);
#define DRV_USB_REG_IPS_TRACECB(x) BSP_USB_RegIpsTraceCB(x)
#endif

/********************************************************
函数说明：协议栈注册USB使能通知回调函输
函数功能:
输入参数：pFunc: USB使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*USB_UDI_ENABLE_CB_T)(void);
extern unsigned int BSP_USB_RegUdiEnableCB(USB_UDI_ENABLE_CB_T pFunc);
#define DRV_USB_REGUDI_ENABLECB(x) BSP_USB_RegUdiEnableCB(x)

/********************************************************
函数说明：协议栈注册USB去使能通知回调函输
函数功能:
输入参数：pFunc: USB使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*USB_UDI_DISABLE_CB_T)(void);
extern unsigned int BSP_USB_RegUdiDisableCB(USB_UDI_DISABLE_CB_T pFunc);
#define DRV_USB_REGUDI_DISABLECB(x) BSP_USB_RegUdiDisableCB(x)

/********************************************************
函数说明：协议栈注册HSIC使能通知回调函输
函数功能:
输入参数：pFunc: HSIC使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*HSIC_UDI_ENABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiEnableCB(HSIC_UDI_ENABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_ENABLECB(x) BSP_HSIC_RegUdiEnableCB(x)

/********************************************************
函数说明：协议栈注册HSIC去使能通知回调函输
函数功能:
输入参数：pFunc: HSIC去使能回调函输指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
typedef void (*HSIC_UDI_DISABLE_CB_T)(void);
extern unsigned int BSP_HSIC_RegUdiDisableCB(HSIC_UDI_DISABLE_CB_T pFunc);
#define DRV_HSIC_REGUDI_DISABLECB(x) BSP_HSIC_RegUdiDisableCB(x)
/********************************************************
函数说明：协议栈查询HSIC枚举状态
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  1：枚举完成
          0：枚举未完成
********************************************************/
extern unsigned int BSP_GetHsicEnumStatus(void);
#define DRV_GET_HSIC_ENUM_STATUS() BSP_GetHsicEnumStatus()

/********************************************************************************************************
 函 数 名  : USB_otg_switch_signal_set
 功能描述  : 用于BALONG和K3对接时USB通道切换GPIO引脚控制
 输入参数  : 组号、引脚号和值
 输出参数  : 无

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_signal_set(UINT8 ucGroup, UINT8 ucPin, UINT8 ucValue);
#define DRV_USB_OTG_SWITCH_SIGNAL_SET(group, pin, value) USB_otg_switch_signal_set(group, pin, value)

#define DRV_GPIO_SET(group, pin, value) USB_otg_switch_signal_set(group, pin, value)
#define DRV_GPIO_USB_SWITCH             200
#define DRV_GPIO_HIGH                   1
#define DRV_GPIO_LOW                    0

/********************************************************************************************************
 函 数 名  : USB_otg_switch_set
 功能描述  : 用于BALONG和K3对接时MODEM侧开启或关闭USB PHY
 输入参数  : 开启或关闭
 输出参数  : 无

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_set(UINT8 ucValue);
#define DRV_USB_PHY_SWITCH_SET(value) USB_otg_switch_set(value)

#define AT_USB_SWITCH_SET_VBUS_VALID            1
#define AT_USB_SWITCH_SET_VBUS_INVALID          2

#define USB_SWITCH_ON                           1
#define USB_SWITCH_OFF                          0

/********************************************************************************************************
 函 数 名  : USB_otg_switch_get
 功能描述  : 用于BALONG和K3对接时MODEM侧USB PHY状态查询
 输入参数  : 无
 输出参数  : 返回USB PHY开启状态

 返 回 值  : 0:  操作成功；
             -1：操作失败。
********************************************************************************************************/
extern int USB_otg_switch_get(UINT8 *pucValue);
#define DRV_USB_PHY_SWITCH_GET(value) USB_otg_switch_get(value)

extern unsigned int USB_ETH_DrvSetDeviceAssembleParam(
    unsigned int ulEthTxMinNum,
    unsigned int ulEthTxTimeout,
    unsigned int ulEthRxMinNum,
    unsigned int ulEthRxTimeout);

extern unsigned int USB_ETH_DrvSetHostAssembleParam(unsigned int ulHostOutTimeout);

/********************************************************
函数说明：协议栈查询HSIC是否支持NCM
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  1：支持NCM
          0：不支持NCM
********************************************************/
extern int BSP_USB_HSIC_SupportNcm(void);
#define DRV_USB_HSIC_SUPPORT_NCM() BSP_USB_HSIC_SupportNcm()

/********************************************************
函数说明：TTF查询预申请SKB Num
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  SKB Num
********************************************************/
extern BSP_U32 BSP_AcmPremallocSkbNum(void);
#define DRV_GET_PREMALLOC_SKB_NUM() BSP_AcmPremallocSkbNum()

/********************************************************
函数名：   BSP_UDI_FindVcom
功能描述： 查询当前设备形态下指定的UDI虚拟串口端口是否存在
输入参数： UDI_DEVICE_ID枚举值，即待查询的虚拟串口端口ID
输出参数： 无
返回值：
           0：当前设备形态不支持查询的虚拟串口端口；
           1：当前设备形态支持查询的虚拟串口端口。
注意事项： 无
********************************************************/
extern int BSP_UDI_FindVcom(UDI_DEVICE_ID enVCOM);

/*****************************************************************************
* 函 数 名  : DRV_USB_RegEnumDoneForMsp
* 功能描述  : 提供给 MSP 注册 USB 枚举完成后通知函数
* 输入参数  : pFunc: 枚举完成回调函数指针
* 输出参数  : 无
* 返 回 值  : 0: 成功注册,等待枚举完成通知;
*             1: USB 已经枚举完成, 可以直接初始化 USB 部分;
*             -1: 失败, 非USB形态,没有USB驱动
*****************************************************************************/
extern signed int BSP_USB_RegEnumDoneForMsp(void *pFunc);
#define DRV_USB_RegEnumDoneForMsp   BSP_USB_RegEnumDoneForMsp

/*****************************************************************************
* 函 数 名  : BSP_USB_RegEnumDoneForPs
* 功能描述  : 提供给 PS 注册 USB 枚举完成后通知函数
* 输入参数  : pFunc: 枚举完成回调函数指针
* 输出参数  : 无
* 返 回 值  : 0: 成功注册,等待枚举完成通知;
*             1: USB 已经枚举完成, 可以直接初始化 USB 部分;
*             -1: 失败, 非USB形态,没有USB驱动
*****************************************************************************/
extern signed int BSP_USB_RegEnumDoneForPs(void *pFunc);
#define DRV_USB_RegEnumDoneForPs    BSP_USB_RegEnumDoneForPs

/********************************************************
函数说明： 返回当前NV项中的设备形态值，是否支持PCSC
函数功能:
输入参数： dev_type是设备形态值（OAM从NV中读出）

输出参数： 无。
返回值：
          0: dev_type不支持PCSC设备；
          1：dev_type支持PCSC设备
********************************************************/
/*unsigned int pcsc_info_quiry(unsigned int ulDevType);*/
#define DRV_USB_PCSC_INFO_QUIRY(a)            DRV_OK


/*闪电卡还未开发 先暂时API 打桩begin*/
/*****************************************************************************
 函 数 名  : BSP_USB_RndisAppEventDispatch
 功能描述  : 本接口用于通知APP 相应的USB插拔事件
 输出参数  : usb事件。
 返 回 值  :无
*****************************************************************************/
extern void BSP_USB_RndisAppEventDispatch(unsigned ulStatus) ;
#define DRV_RNDIS_APP_ENENT_DISPATCH(ulStatus)    BSP_USB_RndisAppEventDispatch(ulStatus)

/************************************************************************
 * FUNCTION
 *       rndis_app_event_dispatch
 * DESCRIPTION
 *       闪电卡版本控制应用进行拨号或断开拨号连接
 * INPUTS
 *       进行拨号或断开拨号指示
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern VOID rndis_app_event_dispatch(unsigned int ulStatus);
extern VOID DRV_AT_SETAPPDAILMODE(unsigned int ulStatus);

/************************************************************************
 * FUNCTION
 *       PDP_ACT_DRV_CALLBACK
 * DESCRIPTION
 *       PDP激活后调用底软的回调函数，原本在闪电卡上用作加载符号表信息，现在打桩
 * INPUTS
 *       NONE
 * OUTPUTS
 *       NONE
 *************************************************************************/
extern int PDP_ACT_DRV_CALLBACK(void);

typedef void(* USB_NET_DEV_SWITCH_GATEWAY)
(
    void
);

/*****************************************************************************
 函 数 名  : BSP_USB_NASSwitchGatewayRegExtFunc
 功能描述  : 本接口用于NAS注册切换网关通知回调函数
 输入参数  :回调接口。
 输出参数  : 无。
 返 回 值  : 0：成功
                       非零:失败
*****************************************************************************/
extern int BSP_USB_NASSwitchGatewayRegFunc(USB_NET_DEV_SWITCH_GATEWAY switchGwMode);
#define DRV_USB_NAS_SWITCH_GATEWAY_REGFUNC(switchGwMode)    BSP_USB_NASSwitchGatewayRegFunc(switchGwMode)
/*闪电卡还未开发 先暂时API 打桩end*/

extern unsigned int USB_ETH_DrvSetRxFlowCtrl(unsigned int ulParam1, unsigned int ulParam2);
extern unsigned int USB_ETH_DrvClearRxFlowCtrl(unsigned int ulParam1, unsigned int ulParam2);

static INLINE BSP_VOID  DRV_AT_SETAPPDIALMODE(unsigned int ulStatus)
{}

#define DRV_COM_RCV_CALLBACK_REGI(uPortNo,pCallback)    DRV_OK

/*宏定义*/
#define USB_L2_ENTER   1
#define USB_L2_EXIT   0

/*函数回调指针定义*/
typedef  void  (*FUNC_USB_LP_NOTIFY)(int iStatus);

/*****************************************************************************
 函 数 名  : DRV_USB_LP_CB_REGISTER
 功能描述  : L2状态进入退出通知回调函数注册接口
 输入参数  : FUNC_USB_LP_NOTIFY *pUSBLPFunc回调函数指针
 输出参数  : 无
 返 回 值  : 0   : 注册成功
             其他：注册失败
*****************************************************************************/
extern int l2_notify_register(FUNC_USB_LP_NOTIFY pUSBLPFunc);
#define DRV_USB_LP_CB_REGISTER(pUSBLPFunc)  \
                l2_notify_register(pUSBLPFunc)


/*
 * ---------------------------
 * PCSC(CCID) 接口
 * ---------------------------
 */
typedef unsigned int (*pFunAPDUProcess)(unsigned int cmd_type, unsigned char *apdu, unsigned int apdu_len);
typedef unsigned int (*GetCardStatus)(void);

typedef struct
{
    pFunAPDUProcess pFuncApdu;
    GetCardStatus   pFuncGetCardStatus;
}BSP_CCID_REGFUNC;

/*****************************************************************************
 函 数 名  : pcsc_usim_ctrl_cmd
 功能描述  : PCSC命令回复函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
unsigned int pcsc_usim_ctrl_cmd(unsigned int cmd_type, unsigned int status,
                unsigned char *buf, unsigned int length);
#define DRV_PCSC_SEND_DATA(CmdType, Result, Buffer, Length) pcsc_usim_ctrl_cmd(CmdType, Result, Buffer, Length)

/*****************************************************************************
 函 数 名  : pcsc_usim_int
 功能描述  : 注册PCSC命令接收函数
 输入参数  :
 输出参数  : None
 返 回 值  : void
*****************************************************************************/
void pcsc_usim_int(pFunAPDUProcess pFun1, GetCardStatus pFun2);
#define DRV_PCSC_REG_CALLBACK(pFun1, pFun2) pcsc_usim_int((pFun1), (pFun2))

#endif

