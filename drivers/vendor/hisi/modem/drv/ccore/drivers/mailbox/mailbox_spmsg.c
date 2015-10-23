

#include "mailbox.h"

#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*lint -save -e958*/
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
void BSP_MailBox_SpULMsgHeadInit(u32 ulAddr, 
                                 u32 ulSize, 
                                 u8 ucData)
{
    s32 key;

    key = intLock();
    memset((void*)ulAddr, ucData, ulSize);
    intUnlock(key);
}

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
void BSP_MailBox_SpDLMsgHeadInit(u32 ulAddr, 
                                 u32 ulSize, 
                                 u8 ucData)
{
    s32 key;

    key = intLock();
    memset((u8*)ulAddr, ucData, ulSize);
    intUnlock(key);
}
/*lint -restore*/

/*****************************************************************************
 Prototype      : MailBox_SpMsgWrite
 Description    : 专有邮箱写接口封装
 Input          : 
                ulAddr : 专有邮箱的地址
                ulSize : 缓冲区大小(以字节为单位)
                pData  : 缓冲区指针(不包含消息头信息)
 Return Value   : 
                0: 表示操作成功
                其他: 表示操作失败
*****************************************************************************/
u32 MailBox_SpMsgWrite(u32 ulAddr, 
                           u32 ulSize, 
                           void* pData)
{
    u32 ret = BSP_OK;
    s32 key;

    key = intLock();
    if(*((u32*)ulAddr))
    {
        /* 设置为无数据 */
        *((u32*)ulAddr) = 0;
        
        ret = ERR_MAILBOX_COVER;
    }
    
    memcpy(((u8*)ulAddr + 4), pData, ulSize);
    
    /* 设置为DSP可以读 */
    *((u32*)ulAddr) = 1;
    
    intUnlock(key);

    return ret;
}

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
u32 BSP_MailBox_SpMsgWrite(u32 ulAddr, 
                               u32 ulSize, 
                               void* pData)
{
    s32 key = 0;
    u32 ret = 0;

    if(!(g_stMbxCtrl.bMbxInit))
    {
        g_stMbxMntn.stAbnormal.ulNInitSlic = BSP_GetSliceValue();
        return ERR_MAILBOX_NOT_INIT;
    }

    if((BSP_NULL == pData) || (0 == ulSize))
    {
        return ERR_MAILBOX_PARAM_INCORRECT;
    }

    key = intLock();
    
    ret = MailBox_SpMsgWrite(ulAddr, ulSize, pData);

    intUnlock(key);

    return ret;
}


/*****************************************************************************
 Prototype      : MailBox_SpMsgWrite
 Description    : 专有邮箱读接口封装
 Input          : 
                ulAddr : 专有邮箱的偏移地址（从消息头开始的地址）
                ulSize : 初始值的大小(以字节为单位)
                pData  : 数据信息(不包含消息头信息)
 Return Value   : 
                0: 表示操作成功
                其他: 表示操作失败
*****************************************************************************/
u32 MailBox_SpMsgRead(u32 ulAddr, 
                          u32 ulSize, 
                          void* pData)
{
    s32 key;

    key = intLock();

    /* 如果DSP已睡眠，表示邮箱中无数据 */
    /* 如果DSP没睡眠，由锁中断保证不调度低功耗模块对DSP的下电处理 */
    if(BSP_TRUE == BSP_MailBox_IsDspSleep()) /*lint !e746*/
    {
        intUnlock(key);
        return ERR_MAILBOX_READ_NULL;
    }
    
    /* 判断当前消息是否已读走 */
    if(0 == *((u32*)ulAddr))
    {
        intUnlock(key);
        return ERR_MAILBOX_READ_NULL;
    }

    memcpy(pData, ((u8*)ulAddr + 4), ulSize);

    /* 设置可以DSP可以读 */
    *((u32*)ulAddr) = 0;
    
    intUnlock(key);

    return BSP_OK;
}

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
u32 BSP_MailBox_SpMsgRead(u32 ulAddr, 
                              u32 ulSize, 
                              void* pData)
{
    s32 key = 0;
    u32 ret = 0;

    if(!(g_stMbxCtrl.bMbxInit))
    {
        g_stMbxMntn.stAbnormal.ulNInitSlic = BSP_GetSliceValue();
        return ERR_MAILBOX_NOT_INIT;
    }

    if((BSP_NULL == pData) || (0 == ulSize))
    {
        return ERR_MAILBOX_PARAM_INCORRECT;
    }

    key = intLock();
    
    ret = MailBox_SpMsgRead(ulAddr, ulSize, pData);

    intUnlock(key);

    return ret;
}

/*****************************************************************************
 Prototype       : BSP_MailBox_SpNotifyReg
 Description     : 专有邮箱数据到达通知注册接口
 param           : pFun             邮箱数据到达处理函数
 Return Value    : BSP_OK: 成功
                   其他: 失败
*****************************************************************************/
u32 BSP_MailBox_SpNotifyReg(BSP_MBX_NOTIFY_FUNC pFun)
{
    g_stMbxCtrl.pfnSpNotifyProc = pFun;

    return BSP_OK;
}

/*****************************************************************************
 Prototype       : BSP_MailBox_SpNotifyEnable
 Description     : 专有邮箱数据到达通知中断使能接口
 param           : None
 Return Value    : void
*****************************************************************************/
void BSP_MailBox_SpNotifyEnable(void)
{
    if(BSP_OK != BSP_IPC_IntConnect(IPC_INT_DSP_PS_MAC_MBX, (VOIDFUNCPTR)Mbx_SpIntProc, 0))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MBX, "[mailbox]: <%s> BSP_IPC_IntConnect failed.\n", __FUNCTION__);
    }

    if(BSP_OK != BSP_IPC_IntEnable(IPC_INT_DSP_PS_MAC_MBX))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MBX, "[mailbox]: <%s> BSP_IPC_IntEnable failed.\n", __FUNCTION__);
    }
}

/*****************************************************************************
 Prototype       : BSP_MailBox_SpNotifyDisable
 Description     : 专有邮箱数据到达通知中断去使能接口
 param           : None
 Return Value    : void
*****************************************************************************/
void BSP_MailBox_SpNotifyDisable(void)
{
    if(BSP_OK != BSP_IPC_IntDisable(IPC_INT_DSP_PS_MAC_MBX))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MBX, "[mailbox]: <%s> BSP_IPC_IntDisable failed.\n", __FUNCTION__);
    }

    if(BSP_OK != BSP_IPC_IntDisonnect(IPC_INT_DSP_PS_MAC_MBX, NULL, 0))
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_MBX, "[mailbox]: <%s> BSP_IPC_IntDisonnect failed.\n", __FUNCTION__);
    }
}


/*****************************************************************************
 Prototype       : Mbx_SpIntProc
 Description     : 专有邮箱中断处理函数
                   (如果DSP写邮箱后通过浮动中断通知ARM，DSP应保证在ARM未读走数据前不休眠)
 param           : None
 Return Value    : None
*****************************************************************************/
void Mbx_SpIntProc(void)
{
    MAILBOX_INT_TYPE_E enType;
    
    g_stMbxMntn.stNumber.ulSpIntNum++;

    if(g_stMbxCtrl.pfnSpNotifyProc)
    {
        enType = g_stMbxCtrl.bIsSubFrameInt ? EN_MAILBOX_INT_SUBFRAME : EN_MAILBOX_INT_SP_IPC;
        g_stMbxCtrl.pfnSpNotifyProc(enType);
    }

    return ;
}


#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif

// end C file
