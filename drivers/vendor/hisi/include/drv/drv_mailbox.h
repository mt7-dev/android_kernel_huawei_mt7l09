/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  drv_mailbox.h
*
*   作    者 :  cuijunqiang
*
*   描    述 :  本文件命名为"drv_mailbox.h", 给出V7R2底软的邮箱接口
*
*   修改记录 :  2013年1月23日  v1.00  cuijunqiang创建
*************************************************************************/

#ifndef __DRV_MAILBOX_H__
#define __DRV_MAILBOX_H__

#ifdef __cplusplus
extern "C" 
{
#endif

#include "product_config.h"

#include "drv_comm.h"
#include "drv_ipc_enum.h"


/* TDS&LTE mailbox driver begin */

typedef enum tagMAILBOX_SLEEP_PROC_E
{
    EN_MAILBOX_SLEEP_WAKEUP = 0,        /* 强制唤醒DSP后处理请求 */
    EN_MAILBOX_SLEEP_WAIT,              /* DSP自动唤醒后处理请求 */
    EN_MAILBOX_SLEEP_LOST,              /* DSP处于睡眠模式时丢弃该请求 */
    EN_MAILBOX_SLEEP_BUTT
}MAILBOX_SLEEP_PROC_E;

typedef enum tagMAILBOX_SERVICE_TYPE_E
{
    EN_MAILBOX_SERVICE_LTE_PS = 0,      /* LTE PS请求 */
    EN_MAILBOX_SERVICE_LTE_CT,          /* LTE CT请求 */
    EN_MAILBOX_SERVICE_LTE_BT,          /* LTE BT请求 */
    EN_MAILBOX_SERVICE_LTE_OM,          /* LTE OM邮箱请求 */
    EN_MAILBOX_SERVICE_LTE_COEX,        /* LTE WIFI coex */
    EN_MAILBOX_SERVICE_LTE_HS_DIAG,     /* LTE 可维可测低功耗数采请求 */

    EN_MAILBOX_SERVICE_TDS_PS,          /* TDS PS请求 */
    EN_MAILBOX_SERVICE_TDS_CT,          /* TDS CT请求 */
    EN_MAILBOX_SERVICE_TDS_BT,          /* TDS BT请求 */
    EN_MAILBOX_SERVICE_TDS_OM,          /* TDS OM邮箱请求 */
    EN_MAILBOX_SERVICE_TDS_HS_DIAG,     /* TDS 可维可测低功耗数采请求 */

    EN_MAILBOX_SERVICE_RTT_SYS_CTRL,    /* DSP可维可测控制业务 */

    EN_MAILBOX_SERVICE_BUTT
}MAILBOX_SERVICE_TYPE_E;


typedef enum tagMAILBOX_INT_TYPE_E
{
    EN_MAILBOX_INT_SUBFRAME = 0,        /* 子帧中断 */
    EN_MAILBOX_INT_COM_IPC,             /* IPC中断 */
    EN_MAILBOX_INT_SP_IPC,              /* 专有邮箱IPC中断 */
    EN_MAILBOX_INT_BUTT
}MAILBOX_INT_TYPE_E;


typedef enum tagMAILBOX_ERRORCODE_E
{
    ERR_MAILBOX_NOT_INIT = 1,           /* 未初始化 */
    ERR_MAILBOX_PARAM_INCORRECT,        /* 参数错误 */
    ERR_MAILBOX_MEMORY_NOTENOUGH,       /* 空间不足 */
    ERR_MAILBOX_TIMEOUT,                /* 超时 */
    ERR_MAILBOX_COVER,                  /* 专有邮箱写操作时原语未读走，执行覆盖操作 */
    ERR_MAILBOX_WRITE_FAILED,           /* 邮箱写失败 */
    ERR_MAILBOX_READ_FAILED,            /* 邮箱读失败 */
    ERR_MAILBOX_READ_NULL,              /* 邮箱读到空信息 */
    ERR_MAILBOX_DSP_POWERDOWN,          /* DSP处于低功耗状态 */
    ERR_MAILBOX_OTHER,                  /* 其他错误 */
    ERR_MAILBOX_ERRORCODE_BUTT
}MAILBOX_ERRORCODE_E;


/*****************************************************************************
 Prototype      : BSP_MailBox_ComMsgInit
 Description    : 通用邮箱的初始化(提供给协议栈调用，
                  接口内部实现与DSP握手信息的初始化)
 Input          : None
 Return Value   : None
*****************************************************************************/
BSP_VOID BSP_MailBox_ComMsgInit(BSP_U32 ulworkingmode);


/*****************************************************************************
 Prototype      : BSP_MailBox_IsDspOk
 Description    : 与DSP的握手函数，判断DSP是否初始化结束
                  (提供给协议栈调用，如果握手不成功，协议栈需要循环多次调用)
 Input          : None
 Return Value   : BSP_TRUE: DSP初始化结束，握手成功
                  BSP_FALSE:DSP没有初始化结束，握手不成功
*****************************************************************************/
BSP_BOOL BSP_MailBox_IsDspOk(BSP_VOID);

/*****************************************************************************
 Prototype       : BSP_MailBox_ComMsgWrite
 Description     : 邮箱写接口(每次只支持写一条原语)
 Input           : enMbxType    邮箱的业务类型
                   pData        数据指针
                   ulLen        数据长度(以字节为单位)
                   enProcType   DSP睡眠时的处理类型
 Output          : None.
 Return Value    : BSP_OK: 成功
                   其他: 失败
*****************************************************************************/
BSP_U32 BSP_MailBox_ComMsgWrite(MAILBOX_SERVICE_TYPE_E enMbxType,
                               BSP_VOID* pData,
                               BSP_U32 ulLen,
                               MAILBOX_SLEEP_PROC_E enProcType);

typedef BSP_VOID (*BSP_MBX_NOTIFY_FUNC)(MAILBOX_INT_TYPE_E enIntType);

/*****************************************************************************
 Prototype       : BSP_MailBox_ComNotifyReg
 Description     : 邮箱数据到达通知注册接口（子帧中断即使没数据也要回调）
 param           : enMbxType        邮箱的业务类型
                   pFun             邮箱数据到达处理函数
 Return Value    : BSP_OK: 成功
                   ERR_MAILBOX_NOT_INIT
                   ERR_MAILBOX_PARAM_INCORRECT
*****************************************************************************/
BSP_U32 BSP_MailBox_ComNotifyReg(MAILBOX_SERVICE_TYPE_E enMbxType,
                                BSP_MBX_NOTIFY_FUNC pFun);

/*****************************************************************************
 Prototype       : BSP_MailBox_IntTypeSet
 Description     : 设置PS邮箱中断源接口(默认IPC中断方式)
                   只用于PS邮箱，OM邮箱无中断上报
 param           : None
 Return Value    : 0: 成功; 其他: 设置的type非法
*****************************************************************************/
BSP_U32 BSP_MailBox_IntTypeSet(MAILBOX_INT_TYPE_E enIntType);

/*****************************************************************************
 Prototype       : BSP_MailBox_ComMsgSize
 Description     : 获取通用邮箱中第一条原语的长度
                   (提供给上层调用，驱动不提供邮箱中原语条数接口，需要上层循环
                   读，以避免上层漏掉中断通知时造成邮箱中原语缓存)
 param           : enMbxType        邮箱的业务类型
 Return Value    : 0:  没有获取原语（邮箱为空，或有其他类型的原语未读走）
                   其他值: 邮箱第一条原语的长度
*****************************************************************************/
BSP_U32 BSP_MailBox_ComMsgSize(MAILBOX_SERVICE_TYPE_E enMbxType);

/*****************************************************************************
 Prototype       : BSP_MailBox_ComMsgRead
 Description     : 邮箱读接口
                   (此接口会唤醒DSP，需要BSP_MailBox_ComMsgSize返回非空时调用)
 param           : enMbxType    邮箱的业务类型
                   pData        接收的数据缓冲区指针(存储原语信息，不包含原语长度)
                   ulLen        pData缓冲区的size(以字节为单位)
                   enProcType   DSP睡眠时的处理类型
 Return Value    : 0 : 成功
                   其他 : 失败
*****************************************************************************/
BSP_U32 BSP_MailBox_ComMsgRead(MAILBOX_SERVICE_TYPE_E enMbxType,
                              BSP_VOID* pData,
                              BSP_U32 ulLen,
                               MAILBOX_SLEEP_PROC_E enProcType);


/*****************************************************************************
 Prototype      : BSP_MailBox_SpULMsgHeadInit
 Description    : 专有（Special Mailbox）上行邮箱的消息头初始化接口
                  (初始化上行邮箱每条原语的消息头，提供给协议栈，需要调用多次
                  由调用者保证调用接口时DSP处于非睡眠状态)
 Input          :
                ulAddr : 专有邮箱的地址
                ulSize : 初始值的大小(以字节为单位)
                ucData : 初始值
 Return Value   None
*****************************************************************************/
BSP_VOID BSP_MailBox_SpULMsgHeadInit(BSP_U32 ulAddr,
                                 BSP_U32 ulSize,
                                 BSP_U8 ucData);

/*****************************************************************************
 Prototype      : BSP_MailBox_SpDLMsgHeadInit
 Description    : 专有（Special Mailbox）下行邮箱的消息头初始化接口
                  (初始化上行邮箱每条原语的消息头，提供给协议栈，需要调用多次
                  由调用者保证调用接口时DSP处于非睡眠状态)
 Input          :
                ulAddr : 专有邮箱的地址
                ulSize : 初始值的大小(以字节为单位)
                ucData : 初始值
 Return Value   None
*****************************************************************************/
BSP_VOID BSP_MailBox_SpDLMsgHeadInit(BSP_U32 ulAddr,
                                 BSP_U32 ulSize,
                                 BSP_U8 ucData);

/*****************************************************************************
 Prototype      : BSP_MailBox_SpMsgWrite
 Description    : 专有邮箱（Special Mailbox）的写接口
                  (由调用者保证调用接口时DSP处于非睡眠状态)
 Input          :
                ulAddr : 专有邮箱的偏移地址（从消息头开始的地址）
                ulSize : 初始值的大小(以字节为单位)
                pData  : 数据信息(不包含消息头信息)
 Return Value   :
                BSP_OK: 表示操作成功
                ERR_MAILBOX_COVER: 表示前一条原语未被读走，覆盖
                ERR_MAILBOX_WRITE_FAILED: 写失败
                ERR_MAILBOX_NOT_INIT: 邮箱未初始化
                ERR_MAILBOX_PARAM_INCORRECT: 参数错误
*****************************************************************************/
BSP_U32 BSP_MailBox_SpMsgWrite(BSP_U32 ulAddr,
                               BSP_U32 ulSize,
                               BSP_VOID* pData);

/*****************************************************************************
 Prototype      : BSP_MailBox_SpMsgRead
 Description    : 专有邮箱（Special Mailbox）的读接口
                  (由调用者保证调用接口时DSP处于非睡眠状态)
 Input          :
                ulAddr : 专有邮箱的偏移地址（从消息头开始的地址）
                ulSize : 初始值的大小(以字节为单位)
                pData  : 数据信息(不包含消息头信息)
 Return Value   :
                BSP_OK: 读取成功
                ERR_MAILBOX_READ_NULL: 无数据
                ERR_MAILBOX_NOT_INIT: 邮箱未初始化
                ERR_MAILBOX_PARAM_INCORRECT: 参数错误
*****************************************************************************/
BSP_U32 BSP_MailBox_SpMsgRead(BSP_U32 ulAddr,
                              BSP_U32 ulSize,
                              BSP_VOID* pData);

/*****************************************************************************
 Prototype       : BSP_MailBox_SpNotifyReg
 Description     : 专有邮箱数据到达通知注册接口
 param           : pFun             邮箱数据到达处理函数
 Return Value    : BSP_OK: 成功
                   其他: 失败
*****************************************************************************/
BSP_U32 BSP_MailBox_SpNotifyReg(BSP_MBX_NOTIFY_FUNC pFun);

/*****************************************************************************
 Prototype       : BSP_MailBox_SpNotifyEnable
 Description     : 专有邮箱数据到达通知中断使能接口
 param           : None
 Return Value    : void
*****************************************************************************/
BSP_VOID BSP_MailBox_SpNotifyEnable(BSP_VOID);

/*****************************************************************************
 Prototype       : BSP_MailBox_SpNotifyDisable
 Description     : 专有邮箱数据到达通知中断去使能接口
 param           : None
 Return Value    : void
*****************************************************************************/
BSP_VOID BSP_MailBox_SpNotifyDisable(BSP_VOID);


/*****************************************************************************
 Prototype       : BSP_Mailbox_ForbidDspSleep
 Description     : 禁止DSP睡眠接口
 Input           : enProcType     DSP睡眠时的处理类型
                    EN_MAILBOX_SLEEP_WAKEUP : 强制唤醒时会有等待处理，
                                              不能在中断回调中输入此参数
                                              只能在任务中输入此参数
                    EN_MAILBOX_SLEEP_LOST   : 中断回调和任务中都可输入此参数

 Return Value    : BSP_OK: 成功,DSP当前没睡眠，且已设置禁止DSP进入睡眠
                   ERR_MAILBOX_DSP_POWERDOWN : 当输入为EN_MAILBOX_SLEEP_LOST,
                                               DSP当前处于低功耗状态,设置失败
                   ERR_MAILBOX_TIMEOUT       : 当输入为EN_MAILBOX_SLEEP_WAKEUP,
                                               唤醒DSP超时(10s)
*****************************************************************************/
BSP_U32 BSP_Mailbox_ForbidDspSleep(MAILBOX_SLEEP_PROC_E enProcType);


/*****************************************************************************
 Prototype       : BSP_Mailbox_AllowDspSleep
 Description     : 允许DSP睡眠接口
 Input           : None
 Return Value    : None
*****************************************************************************/
BSP_VOID BSP_Mailbox_AllowDspSleep(BSP_VOID);



typedef BSP_BOOL (*BSP_MBX_GET_STATUS_FUNC)(BSP_VOID);

/*****************************************************************************
 Prototype       : BSP_MailBox_GetDspStatusReg
 Description     : 获取DSP睡眠状态回调函数注册接口，低功耗模块调用
 param           : pFun             获取DSP睡眠状态处理函数
 Return Value    : BSP_VOID
*****************************************************************************/
BSP_VOID BSP_MailBox_GetDspStatusReg(BSP_MBX_GET_STATUS_FUNC pFun);


typedef BSP_VOID (*BSP_MBX_DSP_AWAKE_FUNC)(BSP_VOID);

/*****************************************************************************
 Prototype       : BSP_MailBox_DspForceAwakeReg
 Description     : DSP强制唤醒回调函数注册接口，低功耗模块调用
 param           : pFun             DSP强制唤醒处理函数
 Return Value    : BSP_VOID
*****************************************************************************/
BSP_VOID BSP_MailBox_DspForceAwakeReg(BSP_MBX_DSP_AWAKE_FUNC pFun);


/*****************************************************************************
 Prototype       : BSP_MailBox_GetSpMsgStatusReg
 Description     : 获取专有邮箱允许DSP睡眠状态回调函数注册接口，PS调用
 param           : pFun             获取DSP睡眠状态处理函数
 Return Value    : BSP_VOID
*****************************************************************************/
BSP_VOID BSP_MailBox_GetSpMsgStatusReg(BSP_MBX_GET_STATUS_FUNC pFun);


/*****************************************************************************
 Prototype       : BSP_MailBox_DspAwakeProc
 Description     : DSP唤醒后的邮箱处理接口，低功耗模块调用
 param           : BSP_VOID
 Return Value    : BSP_VOID
*****************************************************************************/
BSP_VOID BSP_MailBox_DspAwakeProc(BSP_VOID);

/*****************************************************************************
 Prototype       : BSP_MailBox_IsAllowDspSleep
 Description     : 查询是否允许DSP睡眠的处理接口，低功耗模块调用
                    调用此接口时低功耗模块保证DSP未睡眠
                    此接口只检测PS邮箱和专有邮箱，不检查OM邮箱(OM原语无时延要求)
 param           : BSP_VOID
 Return Value    : BSP_BOOL: TRUE-allow;FALSE-forbid
*****************************************************************************/
BSP_BOOL BSP_MailBox_IsAllowDspSleep(BSP_VOID);


/*****************************************************************************
 Prototype       : BSP_UniMailboxWrite
 Description     : 邮箱的通用写接口(由调用者保证传入的地址的合法性)
                   本接口实现类似memcpy函数的作用
                   接口内部保证DSP的唤醒操作，如果唤醒失败，返回ERR_MAILBOX_TIMEOUT
 Return Value    :
                   BSP_OK
                   ERR_MAILBOX_PARAM_INCORRECT
                   ERR_MAILBOX_TIMEOUT
*****************************************************************************/
BSP_U32 BSP_UniMailboxWrite(BSP_VOID *pDst, BSP_VOID *pSrc, BSP_U32 ulLen);

/*****************************************************************************
 Prototype       : BSP_MailboxAbnormalProc
 Description     : 邮箱异常处理
                   协议栈接收消息超时后调用此接口保存邮箱相关信息
                   与协议栈商定，前0.5K保存协议栈信息，后3.5K保存邮箱信息
 Input           : ucData       缓存首地址
 Return Value    : NONE
*****************************************************************************/
BSP_VOID BSP_MailboxAbnormalProc(BSP_U8 *ucData);

/* TDS&LTE mailbox driver end */

#ifdef __cplusplus
}
#endif

#endif

