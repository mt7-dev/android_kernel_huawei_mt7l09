

#ifndef __DRV_CPUFREQ_H__
#define __DRV_CPUFREQ_H__


#include <product_config.h>
#include <soc_cpufreq.h>
#include "drv_comm.h"



#define BALONG_FREQ_MAX			(DC_RESV - 1)
#define BALONG_FREQ_MIN			BALONG_FREQ_PRO0
#define BALONG_FREQ_MAX_DDR	(800)

/*锁定调频接口入参*/
#define BALONG_FREQ_LOCK			(0)
#define BALONG_FREQ_UNLOCK			(1)


/*
 * 获取当前profile
 * 返回当前profile
 */
int pwrctrl_dfs_get_profile(void);


/*
 * 设置profile
 * success: return BSP_OK
 * fail:    return BSP_ERROR
 */
int pwrctrl_dfs_set_profile(int profile);


/*
 * 设置profile下限
 * success: return BSP_OK
 * fail:    return BSP_ERROR
 */
int pwrctrl_dfs_set_baseprofile(int baseprofile);

 

/*
 * 锁定调频 lock=0锁定;lock=1解锁
 */
void pwrctrl_dfs_lock(unsigned int lock_frq);


/*
 * 设置频率 BSP_ERROR 设置失败;BSP_OK 设置成功
 * 入参分别为当前核的cpu频率，ddr频率，slowbus频率
 * 注:因C核profile4、5的CPU频率相同，
 * 若想调至最高频请将ccpu频率设为最高频+1
 * 释放时或取消频率下限限制请设置入参为0
 */
int pwrctrl_dfs_target(int a9freq, int ddrfreq, int slowfreq);
/*
 * 获取当前频率 BSP_ERROR 设置失败;BSP_OK 设置成功
 * 
 */
int pwrctrl_dfs_current(int *a9freq, int *ddrfreq, int *slowfreq);

/*
 * 该接口负责cpu负载检测，
 * 并根据预设阈值判决是否需要向M3请求调频
 * 上调请求调整到最大频率，下调请求调整一个profile
 */
void cpufreq_update_frequency(void);

/********************************adp interface start**************************************/

/*wangwei dfs*/
typedef  enum
{
    PWRCTRL_DFS_USB = 0,
    PWRCTRL_DFS_SD  = 1,
    PWRCTRL_DFS_PS1 = 2,
    PWRCTRL_DFS_HIFI = 3,  /*hifi*/
    PWRCTRL_DFS_BUTT
} PWC_DFS_ID_E;

typedef enum
{
    DFS_PROFILE_0 = 0,            /*频率级别1*/
    DFS_PROFILE_1 = 1,            /*频率级别2*/
    DFS_PROFILE_2 = 2,            /*频率级别3*/
    DFS_PROFILE_3 = 3,            /*频率级别4*/
    DFS_PROFILE_4 = 4,            /*频率级别5*/
    DFS_PROFILE_BUTT
} PWC_DFS_PROFILE_E;

/*
 *  DFS Device QoS
 */
typedef enum tagDFS_QOS_ID_E
{
    DFS_QOS_ID_ACPU_MINFREQ_E = 0,  /*(0)*/
    DFS_QOS_ID_ACPU_MAXFREQ_E,      /*(1)*/
    DFS_QOS_ID_CCPU_MINFREQ_E,      /*(2)*/
    DFS_QOS_ID_CCPU_MAXFREQ_E,      /*(3)*/
    DFS_QOS_ID_DDR_MINFREQ_E,       /*(4)*/
    DFS_QOS_ID_DDR_MAXFREQ_E,       /*(5)*/
    DFS_QOS_ID_GPU_MINFREQ_E,       /*(6)*/
    DFS_QOS_ID_GPU_MAXFREQ_E,       /*(7)*/
    DFS_QOS_ID_BUS_MINFREQ_E,       /*(8)*/
    DFS_QOS_ID_BUS_MAXFREQ_E        /*(9)*/
}DFS_QOS_ID_E;

BSP_S32 PWRCTRL_DfsQosRequest(BSP_S32 qos_id, BSP_U32 req_value, BSP_S32 *req_id);

BSP_S32 PWRCTRL_DfsQosRelease(BSP_S32 qos_id, BSP_S32 *req_id);

BSP_S32 PWRCTRL_DfsQosUpdate(BSP_S32 qos_id, BSP_S32 req_id, BSP_U32 req_value);

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_GetCcpuLoadCB
 功能描述  : arm 提供给TTF的回调函数
 输入参数  : pFunc:TTF函数指针
 输出参数  : 无
 返 回 值  : 申请空间的地址 。
*****************************************************************************/
extern void BSP_PWRCTRL_GetCcpuLoadCB(PWRCTRLFUNCPTR pFunc );
#define DRV_PWRCTRL_GetCcpuLoadCB(pFunc) BSP_PWRCTRL_GetCcpuLoadCB(pFunc)

/*****************************************************************************
 函 数 名  : DRV_PWRCTRL_GetCpuLoad
 功能描述  : arm cpu占有率查询函数
 输入参数  : 无
 输出参数  : ulAcpuLoad:Acpu占有率地址.
             ulCcpuLoad:Ccpu占有率地址.
 返 回 值  : 0/1 。
*****************************************************************************/
extern unsigned int DRV_PWRCTRL_GetCpuLoad(unsigned int *ulAcpuLoad,unsigned int *ulCcpuLoad);

/*****************************************************************************
Function:   DRV_PWRCTRL_DFS_SET_PRF_CCPU
Description:Set the System Min Profile
Input:      ulClientID: The Vote Module Client
            enProfile:  The Min Profile Value
Output:     None
Return:     The Vote Result
Others:
*****************************************************************************/
 BSP_U32  DRV_PWRCTRL_DFS_SET_PRF_CCPU(PWC_DFS_ID_E ulClientID, PWC_DFS_PROFILE_E enProfile);

/*****************************************************************************
Function:   DRV_PWRCTRL_DFS_RLS_PRF_CCPU
Description:Release the Vote Result
Input:      ulClientID: The Vote Module Client
Output:     None;
Return:     The Vote Result
Others:
*****************************************************************************/
 BSP_U32 DRV_PWRCTRL_DFS_RLS_PRF_CCPU(PWC_DFS_ID_E ulClientID);

 /*****************************************************************************
 函 数 名  : BSP_DFS_GetCurCpuLoad
 功能描述  : 查询当前CPU
 输入参数  : pu32AcpuLoad ACPUload指针
             pu32CcpuLoad CCPUload指针
 输出参数  : pu32AcpuLoad ACPUload指针
             pu32CcpuLoad CCPUload指针
 返 回 值  : 0:  操作成功；
            -1：操作失败。
*****************************************************************************/
 BSP_U32 BSP_DFS_GetCurCpuLoad(BSP_U32 *pu32AcpuLoad,BSP_U32 *pu32CcpuLoad);
#define DRV_GET_CUR_CPU_LOAD(pu32AcpuLoad,pu32CcpuLoad) BSP_DFS_GetCurCpuLoad(pu32AcpuLoad,pu32CcpuLoad)

/********************************adp interface end***************************************/

#endif /*__DRV_CPUFREQ_H__*/

