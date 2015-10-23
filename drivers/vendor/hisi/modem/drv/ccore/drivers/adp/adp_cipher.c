

#include <drv_cipher.h>
#include <bsp_cipher.h>
#include <product_config.h>

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_DMA
*
* 功能描述  : 为指定通道配置描述符，用于单纯的DMA搬移
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*             pInMemMgr    : 输入buffer首地址
*             pOutMemMgr   : 输出buffer首地址
* 输出参数  : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_DMA(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,ACC_SINGLE_CFG_S *pstCfg)
{
	return (BSP_S32)bsp_acc_dma((u32)u32BdFifoAddr, (const void *)pInMemMgr, (const void *)pOutMemMgr, pstCfg);
}

/*****************************************************************************
* 函 数 名  : BSP_Accelerator_Cipher
*
* 功能描述  : 为指定通道配置描述符，用于需要Cipher进行加密的操作
*
* 输入参数  : u32BdFifoAddr: BDFIFO首地址
*             pInMemMgr    : 输入buffer首地址
*             pOutMemMgr   : 输出buffer首地址
*             pstCfg       : Cipher配置信息
* 输出参数   : 无
* 返 回 值  : 成功/失败
*****************************************************************************/
BSP_S32 BSP_Accelerator_Cipher(BSP_U32 u32BdFifoAddr, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr, ACC_SINGLE_CFG_S *pstCfg)
{
	return (BSP_S32)bsp_acc_cipher((u32)u32BdFifoAddr, (const void *)pInMemMgr, (const void *)pOutMemMgr, pstCfg);
}

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_RegistNotifyCB
*
* 功能描述  : 注册通知回调函数
*
* 输入参数  : pFunNotifyCb: 通知回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistNotifyCB(CIPHER_NOTIFY_CB_T pFunNotifyCb)
{
	return (BSP_S32)bsp_cipher_regist_notify_func(pFunNotifyCb);
}

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_FreeMemCB
*
* 功能描述  : 注册Memory释放回调函数
*
* 输入参数  : u32Chn: 通道号
*             bSrc:   0:输入Buffer / 1:输出Bufffer
*             pFunFreeMemCb: Memory释放回调函数
*
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*****************************************************************************/
BSP_S32 BSP_CIPHER_RegistFreeMemCB(BSP_U32 u32Chn, BSP_BOOL bSrc, CIPHER_FREEMEM_CB_T pFunFreeMemCb)
{
	return (BSP_S32)bsp_cipher_regist_freemem_func((u32)u32Chn, (s32)bSrc, pFunFreeMemCb);
}

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_Purge
*
* 功能描述  : 清除所有当前指定通道的CIPHER任务(阻塞接口,完成purge后返回)
*
* 输入参数  : u32Chn: 要清除的通道号
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_INVALID_CHN
*           :      CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_Purge(BSP_U32 u32Chn)
{
	return (BSP_S32)bsp_cipher_purge((u32)u32Chn);
}

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_SingleSubmitTask
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于单独操作。
*
* 输入参数  : u32Chn: 通道号
*             pInMemMgr: 输入buffer内存管理结构指针
*             pOutMemMgr:输出buffer内存管理结构指针
*             pstCfg: Cipher操作配置属性
*             pstSubmAttr:提交的属性
*             u32Private:私有数据
* 输出参数  : 无* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_SingleSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr,
	CIHPER_SINGLE_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private)
{
	return (BSP_S32)bsp_cipher_single_submit_task((u32)u32Chn, (const void *)pInMemMgr, (const void *)pOutMemMgr,
		pstCfg, enSubmAttr, (u32)u32Private);
}

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_RelaSubmitTask
*
* 功能描述  : 开始CIPHER 指定通道的工作，用于关联操作。
*
* 输入参数  : u32Chn: 通道号
*             pInMemMgr: 输入buffer内存管理结构指针
*             pOutMemMgr:输出buffer内存管理结构指针
*             pstCfg: Cipher操作配置属性
*             enSubmAttr:提交的属性
*             u32Private:私有数据
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*             失败:CIPHER_NOT_INIT
*             CIPHER_INVALID_CHN
*             CIPHER_NULL_PTR
*             CIPHER_ALIGN_ERROR
*             CIPHER_INVALID_ENUM
*             CIPHER_PURGING
*             CIPHER_FIFO_FULL
*             CIPHER_TIME_OUT
*****************************************************************************/
BSP_S32 BSP_CIPHER_RelaSubmitTask(BSP_U32 u32Chn, const BSP_VOID* pInMemMgr, const BSP_VOID* pOutMemMgr, 
	CIHPER_RELA_CFG_S *pstCfg, CIPHER_SUBMIT_TYPE_E enSubmAttr, BSP_U32 u32Private)
{
	return (BSP_S32)bsp_cipher_rela_submit_task((u32)u32Chn, (const void *)pInMemMgr, (const void *)pOutMemMgr,
		pstCfg, enSubmAttr, (u32)u32Private);
}

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_SetKeyIndex
*
* 功能描述  : 设置密钥值
*
* 输入参数  : pKeyAddr   : 密钥地址
*             enKeyLen   : 密钥长度
*             u32KeyIndex: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:CIPHER_SUCCESS
*           : 失败:CIPHER_NOT_INIT
*           :      CIPHER_NULL_PTR
*           :      CIPHER_INVALID_ENUM
*           :      CIPHER_INVALID_KEY
*****************************************************************************/
BSP_S32 BSP_CIPHER_SetKey(const BSP_VOID* pKeyAddr,CIPHER_KEY_LEN_E enKeyLen,BSP_U32 u32KeyIndex)
{
	return (BSP_S32)bsp_cipher_setkey((const void *)pKeyAddr, enKeyLen, (u32)u32KeyIndex);
}

/*****************************************************************************
* 函 数 名  : BSP_CIPHER_GetKey
*
* 功能描述  : 根据KeyIndex获取Key的地址
*
* 输入参数  : u32KeyIndex: 密钥索引
* 输出参数  : 无
* 返 回 值  : 成功:根据KeyIndex得到的Key的地址
*           : 失败:获取Key值所在地址失败,返回0
*****************************************************************************/
BSP_S32 BSP_CIPHER_GetKey(BSP_U32 u32KeyIndex, KEY_GET_S *pstKeyGet)
{
	return (BSP_S32)bsp_cipher_getkey((u32)u32KeyIndex, pstKeyGet);
}


BSP_S32 BSP_CIPHER_SetDataBufPara(BSP_U32 u32Chn, BSP_BOOL bSrc, BSP_U32 u32BufOft, BSP_U32 u32LenOft, BSP_U32 u32NextOft)
{
	return (BSP_S32)bsp_cipher_set_data_buf_para((u32)u32Chn, (int)bSrc, (u32)u32BufOft,(u32)u32LenOft, (u32)u32NextOft);
}


BSP_S32 BSP_CIPHER_GetCmplSrcBuff(BSP_U32 u32Chn, BSP_U32 *pu32SourAddr, BSP_U32 *pu32DestAddr, CIPHER_NOTIFY_STAT_E *penStatus, BSP_U32 *pu32Private)
{
	return (BSP_S32)bsp_cipher_get_cmpl_src_buff((u32)u32Chn, (u32 *)pu32SourAddr, (u32 *)pu32DestAddr, penStatus, (u32 *)pu32Private);
}


BSP_U32 BSP_Accelerator_GetBdFifoAddr(void)
{
	return (BSP_U32)bsp_acc_get_bdfifo_addr();
}


BSP_S32 BSP_Accelerator_GetStatus(void)
{
	return (BSP_S32)bsp_acc_get_status();
}


BSP_S32 BSP_Accelerator_Enable(BSP_U32 u32BdFifoAddr)
{
	return (BSP_S32)bsp_acc_enable((u32)u32BdFifoAddr);
}


BSP_S32 BSP_KDF_KeyRamRead(BSP_VOID *pDestAddr, BSP_U32 u32KeyIndex ,BSP_U32 u32ReadLength)
{
	return (BSP_S32)bsp_kdf_key_ram_read((void *)pDestAddr, (u32)u32KeyIndex, (u32)u32ReadLength);
}


BSP_S32 BSP_KDF_KeyRamWrite(BSP_U32 u32KeyIndex, BSP_VOID *pSourAddr, BSP_U32 u32Length)
{
	return (BSP_S32)bsp_kdf_key_ram_write((u32)u32KeyIndex, (void *)pSourAddr, (u32)u32Length);
}


BSP_S32 BSP_KDF_KeyMake(KEY_CONFIG_INFO_S *pstKeyCfgInfo, S_CONFIG_INFO_S *pstSCfgInfo, BSP_U32 u32DestIndex, KEY_MAKE_S *pstKeyMake)
{
	return (BSP_S32)bsp_kdf_key_make(pstKeyCfgInfo, pstSCfgInfo, (u32)u32DestIndex, pstKeyMake);
}

#ifdef BSP_CONFIG_HI3630
BSP_S32 BSP_CIPHER_Disable(void)
{
	return (BSP_S32)cipher_close_clk();
}
BSP_S32 BSP_CIPHER_Enable(void)
{
	return (BSP_S32)cipher_open_clk();
}
#else
BSP_S32 BSP_CIPHER_Disable(void)
{
	return 0;
}
BSP_S32 BSP_CIPHER_Enable(void)
{
	return 0;
}
#endif

