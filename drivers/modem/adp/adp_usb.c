/*lint -save -e19 -e123 -e537 -e713*/
#include "drv_usb.h"
#include "bsp_usb.h"
#include "usb_vendor.h"
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/module.h>

static unsigned g_usb_enum_done_cur = 0;
static unsigned g_usb_enum_dis_cur = 0;

static struct notifier_block gs_adp_usb_nb;
static struct notifier_block *gs_adp_usb_nb_ptr = NULL;
static int g_usb_enum_done_notify_complete = 0;
static int g_usb_disable_notify_complete = 0;
static USB_CTX_S g_usb_ctx = {{0},{0},{0}};


/*****************************************************************************
 函 数 名  : BSP_USB_SetPid
 功能描述  :
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
int BSP_USB_SetPid(unsigned char u2diagValue)
{
    return OK;
}

/*****************************************************************************
函数名：   BSP_USB_PortTypeQuery
功能描述:  查询当前的设备枚举的端口形态值
输入参数： stDynamicPidType  端口形态
输出参数： stDynamicPidType  端口形态
返回值：   0:    查询成功
           其他：查询失败
*****************************************************************************/
unsigned int BSP_USB_PortTypeQuery(DRV_DYNAMIC_PID_TYPE_STRU *pstDynamicPidType)
{
    return OK;
}

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
unsigned int BSP_USB_PortTypeValidCheck(unsigned char *pucPortType, unsigned long ulPortNum)
{
    return OK;
}

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
                            unsigned long *pulPortNum, unsigned long ulPortMax)
{
    return OK;
}

/*****************************************************************************
 函 数 名  : BSP_USB_ATProcessRewind2Cmd
 功能描述  : rewind2 CMD 处理
 输入参数  : pData：数据
 输出参数  : 无
 返 回 值  : false(0):处理失败
             tool(1):处理成功
*****************************************************************************/
int BSP_USB_ATProcessRewind2Cmd(unsigned char *pData)
{
    return OK;
}

/*****************************************************************************
 函 数 名  : BSP_USB_GetDiagModeValue
 功能描述  : 获得设备类型
 输入参数  : 无
 输出参数  : ucDialmode:  0 - 使用Modem拨号; 1 - 使用NDIS拨号; 2 - Modem和NDIS共存
              ucCdcSpec:   0 - Modem/NDIS都符合CDC规范; 1 - Modem符合CDC规范;
                           2 - NDIS符合CDC规范;         3 - Modem/NDIS都符合CDC规范
 返 回 值  : VOS_OK/VOS_ERR
*****************************************************************************/
BSP_S32 BSP_USB_GetDiagModeValue(unsigned char *pucDialmode,
                             unsigned char *pucCdcSpec)
{
    return OK;
}

/*****************************************************************************
 函 数 名  : BSP_USB_GetPortMode
 功能描述  : 获取端口形态模式，网关对接需求，打桩。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 无
*****************************************************************************/
unsigned char BSP_USB_GetPortMode(char*PsBuffer, unsigned long*Length )
{
    return OK;
}

/*****************************************************************************
 函 数 名  : BSP_USB_GetU2diagDefaultValue
 功能描述  : 获得端口默认u2diag值
 输入参数  : 无
 输出参数  : 无
 返回值：   u2diag值

*****************************************************************************/
BSP_U32 BSP_USB_GetU2diagDefaultValue(void)
{
    return 0;
}


void MNTN_ERRLOG_REG_FUNC(MNTN_ERRLOGREGFUN pRegFunc)
{
    return ;
}

/*****************************************************************************
 函 数 名  : BSP_USB_UdiagValueCheck
 功能描述  : 本接口用于检查NV项中USB形态值的合法性
 输入参数  : 无
 输出参数  : 无
 返 回 值  : 0：OK
             -1：ERROR
*****************************************************************************/
int BSP_USB_UdiagValueCheck(unsigned long DiagValue)
{
    return OK;
}

/*****************************************************************************
 函 数 名  : BSP_USB_GetLinuxSysType
 功能描述  : 本接口用于检查PC侧是否为Linux，以规避Linux后台二次拨号失败的问题.
 输入参数  : 无。
 输出参数  : 无。
 返 回 值  : 0：Linux；
            -1：非Linux。
*****************************************************************************/
int BSP_USB_GetLinuxSysType(void)
{
    return -1;
}

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
int BSP_USB_PcscInfoSet(unsigned int  ulSel, unsigned int  ulCurDevType, unsigned int *pulDevType)
{
    return 0;
}

/********************************************************
函数说明：协议栈注册USB使能通知回调函数
函数功能:
输入参数：pFunc: USB使能回调函数指针
输出参数：无
输出参数：无
返回值  ：0：成功
          1：失败
********************************************************/
unsigned int BSP_USB_RegUdiEnableCB(USB_UDI_ENABLE_CB_T pFunc)
{
    if (g_usb_enum_done_cur >= USB_ENABLE_CB_MAX)
    {
        printk("BSP_USB_RegUdiEnableCB error:0x%x", (unsigned)pFunc);
        return (unsigned int)ERROR;
    }

    g_usb_ctx.udi_enable_cb[g_usb_enum_done_cur] = pFunc;
    g_usb_enum_done_cur++;

	if (g_usb_enum_done_notify_complete)
    {
    	if (pFunc)
        	pFunc();
    }

    return OK;
}

/********************************************************
函数说明：协议栈注册USB去使能通知回调函数
函数功能:
输入参数：pFunc: USB去使能回调函数指针
输出参数：无
输出参数：无
返回值  ：0：成功
          1：失败
********************************************************/
unsigned int BSP_USB_RegUdiDisableCB(USB_UDI_DISABLE_CB_T pFunc)
{
    if (g_usb_enum_dis_cur >= USB_ENABLE_CB_MAX)
    {
        printk("BSP_USB_RegUdiDisableCB error:0x%x", (unsigned)pFunc);
        return (unsigned int)ERROR;
    }

    g_usb_ctx.udi_disable_cb[g_usb_enum_dis_cur] = pFunc;
    g_usb_enum_dis_cur++;

    return OK;
}

unsigned int BSP_USB_RegIpsTraceCB(USB_IPS_MNTN_TRACE_CB_T pFunc)
{
    if (!pFunc)
    {
        return 1;
    }

    return 0;
}

/********************************************************
函数说明：协议栈注册HSIC使能通知回调函数
函数功能:
输入参数：pFunc: HSIC使能回调函数指针
输出参数：无
输出参数：无
返回值  ：0：成功
          1：失败
********************************************************/
unsigned int BSP_HSIC_RegUdiEnableCB(HSIC_UDI_ENABLE_CB_T pFunc)
{
    return OK;
}

/********************************************************
函数说明：协议栈注册HSIC去使能通知回调函数
函数功能:
输入参数：pFunc: HSIC去使能回调函数指针
输出参数：无
输出参数：无
返回值：  0：成功
          1：失败
********************************************************/
unsigned int BSP_HSIC_RegUdiDisableCB(HSIC_UDI_DISABLE_CB_T pFunc)
{
    return OK;
}

unsigned long USB_ETH_DrvSetDeviceAssembleParam(
    unsigned long ulEthTxMinNum,
    unsigned long ulEthTxTimeout,
    unsigned long ulEthRxMinNum,
    unsigned long ulEthRxTimeout)
{
    return 0;
}

unsigned long USB_ETH_DrvSetHostAssembleParam(unsigned long ulHostOutTimeout)
{
#ifdef CONFIG_BALONG_NCM
    /* the interface don't include net_id, so we just set net_id:0 */
    (void)ncm_set_host_assemble_param(0, ulHostOutTimeout);
#endif
    return 0;
}

int USB_otg_switch_get(UINT8 *pvalue)
{
    return 0;
}

int USB_otg_switch_set(UINT8 value)
{
    return 0;
}

int USB_otg_switch_signal_set(UINT8 group,UINT8 pin, UINT8 value )
{
    return 0;
}

/********************************************************
函数说明：协议栈查询HSIC枚举状态
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  1：枚举完成
          0：枚举未完成
********************************************************/
unsigned int BSP_GetHsicEnumStatus(void)
{
    return OK;
}

/*****************************************************************************
 函 数 名  : BSP_USB_HSIC_SupportNcm
 功能描述  : 本接口用于查询HSIC是否支持NCM
 输入参数  : NA
 输出参数  : NA
 返 回 值  : 1-- 支持NCM
             0--不支持NCM
*****************************************************************************/
int BSP_USB_HSIC_SupportNcm(void)
{
    return 0;
}

/********************************************************
函数说明：TTF查询预申请SKB Num
函数功能:
输入参数：无
输出参数：无
输出参数：无
返回值：  SKB Num
********************************************************/
BSP_U32 BSP_AcmPremallocSkbNum(void)
{
    return 0;
}

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
int BSP_UDI_FindVcom(UDI_DEVICE_ID enVCOM)
{
    return 0;
}

/*****************************************************************************
* 函 数 名  : DRV_USB_RegEnumDoneForMsp
* 功能描述  : 提供给MSP注册USB枚举完成后通知函数
* 输入参数  : pFunc:枚举完成回调函数指针
* 输出参数  : 无
* 返 回 值  : 0: 成功注册,等待枚举完成通知;
*             1: USB已经枚举完成,可以直接初始化USB部分;
*             -1: 失败, 非USB形态,没有USB驱动
*****************************************************************************/
signed int BSP_USB_RegEnumDoneForMsp(void *pFunc)
{
    /* we suggest to use the new interface for usb insert/remove */
    return BSP_USB_RegUdiEnableCB(pFunc);
}

/*****************************************************************************
* 函 数 名  : BSP_USB_RegEnumDoneForPs
* 功能描述  : 提供给PS注册USB枚举完成后通知函数
* 输入参数  : pFunc:枚举完成回调函数指针
* 输出参数  : 无
* 返 回 值  : 0: 成功注册,等待枚举完成通知;
*             1: USB已经枚举完成,可以直接初始化USB部分;
*             -1: 失败,非USB形态,没有USB驱动
*****************************************************************************/
signed int BSP_USB_RegEnumDoneForPs(void *pFunc)
{
    /* we suggest to use the new interface for usb insert/remove */
    return BSP_USB_RegUdiEnableCB(pFunc);
}

/*****************************************************************************
 函 数 名  : BSP_USB_RndisAppEventDispatch
 功能描述  : 本接口用于通知APP 相应的USB插拔事件
 输出参数  : usb事件
 返 回 值  : 无
*****************************************************************************/
void BSP_USB_RndisAppEventDispatch(unsigned ulStatus)
{
    return ;
}

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
VOID rndis_app_event_dispatch(unsigned int ulStatus)
{
    return ;
}

/*****************************************************************************
 函 数 名  : BSP_USB_NASSwitchGatewayRegExtFunc
 功能描述  : 本接口用于NAS注册切换网关通知回调函数
 输入参数  : 回调接口。
 输出参数  : 无。
 返 回 值  : 0：成功
             零:失败
*****************************************************************************/
int BSP_USB_NASSwitchGatewayRegFunc(USB_NET_DEV_SWITCH_GATEWAY switchGwMode)
{
    return OK;
}

/*****************************************************************************
 函 数 名  : bsp_usb_register_enablecb
 功能描述  : 本接口用于Cshell通知USB插入回调函数
 输入参数  : 回调接口。
 输出参数  : 无。
 返 回 值  : 0：成功
             零:失败
*****************************************************************************/
int bsp_usb_register_enablecb(USB_UDI_ENABLE_CB_T pFunc)
{
    /* we suggest to use the new interface for usb insert/remove */
    return BSP_USB_RegUdiEnableCB(pFunc);
}

/*****************************************************************************
 函 数 名  : bsp_usb_register_disablecb
 功能描述  : 本接口用于Cshell通知USB拔出回调函数
 输入参数  : 回调接口。
 输出参数  : 无。
 返 回 值  : 0：成功
             零:失败
*****************************************************************************/
int bsp_usb_register_disablecb(USB_UDI_DISABLE_CB_T pFunc)
{
    /* we suggest to use the new interface for usb insert/remove */
    return BSP_USB_RegUdiDisableCB(pFunc);
}

unsigned long USB_ETH_DrvSetRxFlowCtrl(unsigned long ulParam1, unsigned long ulParam2)
{
    return 0;
}

unsigned long USB_ETH_DrvClearRxFlowCtrl(unsigned long ulParam1, unsigned long ulParam2)
{
    return 0;
}

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
VOID DRV_AT_SETAPPDAILMODE(unsigned int ulStatus)
{

}

int l2_notify_register(FUNC_USB_LP_NOTIFY pUSBLPFunc)
{
    return 0;
}

static int gs_usb_adp_notifier_cb(struct notifier_block *nb,
            unsigned long event, void *priv)
{
    int loop;

    switch (event) {

    case USB_BALONG_DEVICE_INSERT:
        g_usb_disable_notify_complete = 0;
        break;
    case USB_BALONG_ENUM_DONE:
        /* enum done */
        g_usb_disable_notify_complete = 0;
        if (!g_usb_enum_done_notify_complete) {
            for(loop = 0; loop < USB_ENUM_DONE_CB_BOTTEM; loop++)
            {
                if(g_usb_ctx.enum_done_cbs[loop])
                    g_usb_ctx.enum_done_cbs[loop]();
            }

            for(loop = 0; loop < USB_ENABLE_CB_MAX; loop++)
            {
                if(g_usb_ctx.udi_enable_cb[loop])
                    g_usb_ctx.udi_enable_cb[loop]();
            }
        }
        g_usb_enum_done_notify_complete = 1;
        break;
    case USB_BALONG_DEVICE_DISABLE:
    case USB_BALONG_DEVICE_REMOVE:
        /* notify other cb */
        g_usb_enum_done_notify_complete = 0;
        if (!g_usb_disable_notify_complete) {
            for(loop = 0; loop < USB_ENABLE_CB_MAX; loop++)
            {
                if(g_usb_ctx.udi_disable_cb[loop])
                    g_usb_ctx.udi_disable_cb[loop]();
            }
            g_usb_disable_notify_complete = 1;
        }
        break;
    default:
        break;
    }
    return 0;
}


int __init adp_usb_init(void)
{
    /* we just regist once, and don't unregist any more */
    if (!gs_adp_usb_nb_ptr) {
        gs_adp_usb_nb_ptr = &gs_adp_usb_nb;
        gs_adp_usb_nb.priority = USB_NOTIF_PRIO_ADP;
        gs_adp_usb_nb.notifier_call = gs_usb_adp_notifier_cb;
        bsp_usb_register_notify(gs_adp_usb_nb_ptr);
    }
    return 0;
}
module_init(adp_usb_init);
/*lint -restore*/