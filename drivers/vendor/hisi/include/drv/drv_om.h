/*************************************************************************
*   版权所有(C) 1987-2011, 深圳华为技术有限公司.
*
*   文 件 名 :  DrvInterface.h
*
*   作    者 :  yangzhi
*
*   描    述 :  本文件命名为"DrvInterface.h", 给出V7R1底软和协议栈之间的API接口统计
*
*   修改记录 :  2011年1月18日  v1.00  yangzhi创建
*************************************************************************/

#ifndef __DRV_OM_H__
#define __DRV_OM_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "drv_comm.h"


/*************************OM START**********************************/

#define PS_L2_UL_TRACE           (0)
#define PS_L2_DL_TRACE           (1)

#define PROCESS_ENTER(pADDRESS)  do { } while (0)
#define PROCESS_EXIT(pADDRESS)   do { } while (0)

typedef enum{
     ADDRTYPE8BIT,
     ADDRTYPE16BIT,
     ADDRTYPE32BIT
}ENADDRTYPE;

typedef enum{
     BSP_MODEM_CORE         = 0,
     BSP_APP_CORE
}BSP_CORE_TYPE_E;


/*****************************************************************************
  底软主动上报互调函数定义
*****************************************************************************/
typedef BSP_S32 (*log_bin_ind_cb)(BSP_S32 str_id, BSP_VOID* ind_data, BSP_U32 ind_data_size);

/******************************************************************************
* Function     :   BSP_OM_RegRead
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_RegRead(BSP_U32 u32RegAddr, ENADDRTYPE enAddrType, BSP_U32 *pu32Value);


/******************************************************************************
* Function     :   BSP_OM_RegWrite
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_S32 BSP_OM_RegWrite(BSP_U32 u32RegAddr, ENADDRTYPE enAddrType, BSP_U32 u32Value);

/*****************************************************************************
* 函 数 名  : ddmPhaseScoreBoot
*
* 功能描述  : 上层调用的打点计时函数
*
* 输入参数  : phaseName:打点函数名称
*             param:    打点函数所在行号
* 输出参数  : 无
*
* 返 回 值  : 0 成功;其他 失败
*****************************************************************************/
#define ddmPhaseScoreBoot ddm_phase_boot_score
extern int ddm_phase_boot_score(const char * phaseName, unsigned int param);

/*****************************************************************************
* 函 数 名  : DRV_OM_SET_HSO_CONN_FLAG
*
* 功能描述  : flag :连接标志，1表示连接，0表示断开
*
* 输入参数  :无
*
*
* 输出参数  :无
*
* 返 回 值  :无
*****************************************************************************/
BSP_VOID DRV_OM_SET_HSO_CONN_FLAG(BSP_U32 flag);

/*****************************************************************************
* 函 数 名  : DRV_SYSVIEW_SWT_SET
*
* 功能描述  :sysview上报开关设置，封装函数，提供给MSP使用
*
* 输入参数  :tarce_type :trace类型
*                        set_swt  :开关值
*                       period  :上报周期
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/

BSP_U32 DRV_SYSVIEW_SWT_SET(BSP_U32 set_type,BSP_U32 set_swt,BSP_U32 period);

/*****************************************************************************
* 函 数 名  : DRV_GET_ALL_TASK_ID_NAME
*
* 功能描述  :获取当前所有任务的ID和名称
*
* 输入参数  :p_task_stru:任务名称和ID输出的结构体
*                       param_len:参数1的长度，单位字节
*
*
* 输出参数  : 无
*
* 返 回 值  : BSP_OK
*****************************************************************************/
BSP_U32 DRV_GET_ALL_TASK_ID_NAME(BSP_VOID *p_task_stru,BSP_U32 param_len);

/*****************************************************************************
* 函 数 名  : DRV_INT_LOCK
*
* 功能描述  :锁中断接口，用于锁中断统计
*
* 输入参数  :无
*
* 输出参数  : 无
*
* 返 回 值  : 锁中断后获得的key
*****************************************************************************/
BSP_S32 DRV_INT_LOCK(BSP_VOID);

/*****************************************************************************
* 函 数 名  : DRV_INT_UNLOCK
*
* 功能描述  : 解锁中断接口，用于锁中断统计
*
* 输入参数  : LockKey 锁中断后获得的key
*
* 输出参数  : 无
*
* 返 回 值  : 无
*****************************************************************************/
BSP_VOID DRV_INT_UNLOCK(BSP_S32 LockKey);

/*****************************************************************************
* 函 数 名  : DRV_SOCP_CHAN_ENABLE
*
* 功能描述  : 底软SOCP通道使能操作
*
* 输入参数  : 无
*
*
* 输出参数  :无
*
* 返 回 值  :无
*****************************************************************************/

BSP_VOID DRV_SOCP_CHAN_ENABLE(BSP_VOID);

/******************************************************************************
* Function     :   BSP_OM_SoftReboot
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
BSP_VOID BSP_OM_SoftReboot(void);

/*****************************************************************************
* 函 数 名  : BSP_GetHostCore
*
* 功能描述  : 查询当前CPU主从核类型
*
* 输入参数  : 无
*
* 输出参数  : 无
*
* 返 回 值  : CPU主从核类型
*
* 其它说明  : 无
*
*****************************************************************************/
BSP_CORE_TYPE_E BSP_GetHostCore(BSP_VOID);

typedef struct tagBSP_OM_NET_S
{
    BSP_U32 u32NetRxStatOverFlow;       /* 接收FIFO溢出统计计数 */
    BSP_U32 u32NetRxStatPktErr;         /* 接收总错包计数 */
    BSP_U32 u32NetRxStatCrcErr;         /* 接收CRC错包计数 */
    BSP_U32 u32NetRxStatLenErr;         /* 接收无效长度包计数 */
    BSP_U32 u32NetRxNoBufInt;           /* 接收没有BUFFER中断计数 */
    BSP_U32 u32NetRxStopInt;            /* 接收停止中断计数 */
    BSP_U32 u32NetRxDescErr;            /* 接收描述符错误 */

    BSP_U32 u32NetTxStatUnderFlow;      /* 发送FIFO下溢统计计数 */
    BSP_U32 u32NetTxUnderFlowInt;       /* 发送FIFO下溢中断计数 */
    BSP_U32 u32NetTxStopInt;            /* 发送停止中断计数 */
    BSP_U32 u32NetTxDescErrPs;          /* 发送描述符错误(Ps) */
    BSP_U32 u32NetTxDescErrOs;          /* 发送描述符错误(Os) */
    BSP_U32 u32NetTxDescErrMsp;         /* 发送描述符错误(Msp) */

    BSP_U32 u32NetFatalBusErrInt;      /* 总线错误*/
}BSP_OM_NET_S;

/******************************************************************************
* Function     :   BSP_OM_NET
*
* Description  :
*
* Input        :
*
* Output       :   无
*
* return       :
******************************************************************************/
static INLINE BSP_VOID BSP_OM_NET(BSP_OM_NET_S *pstNetOm)
{
	pstNetOm = pstNetOm;
}

/*****************************************************************************
* 函 数 名  : BSP_GetMacAddr
*
* 功能描述  : 获取ps mac地址
*
* 输入参数  :
* 输出参数  : 无
*
* 返 回 值  : VOID
*
* 修改记录  : 2009年9月2日   lvhui  creat
*****************************************************************************/
static INLINE BSP_U8* BSP_GetMacAddr(void)  //clean warning
{
    /* LTE协议栈MAC地址 */
    static BSP_U8  g_mac_address_pstable[] =
    {
        //0x00,0x1a,0x2b,0x3c,0x4d,0x5f
        0x4c,0x54,0x99,0x45,0xe5,0xd5
    };

    return g_mac_address_pstable;
}

#define DRV_MSP_PROC_REG(eFuncID, pFunc)
int DRV_GET_FS_ROOT_DIRECTORY(char * data, unsigned int len);


/*************************OM END  **********************************/


#ifdef __cplusplus
}
#endif

#endif

