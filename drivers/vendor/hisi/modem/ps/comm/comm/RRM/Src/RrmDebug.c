



/*****************************************************************************
  1 头文件包含
*****************************************************************************/
#include "RrmDebug.h"



#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif

/*****************************************************************************
    协议栈打印打点方式下的.C文件宏定义
*****************************************************************************/
#define    THIS_FILE_ID        PS_FILE_ID_RRM_DEBUG_C

#if (FEATURE_ON == FEATURE_DSDS)

RRM_STATS_INFO_STRU                     g_stRrmStats;



VOS_VOID  RRM_InitDebugInfo(VOS_VOID)
{

    PS_MEM_SET(&g_stRrmStats, 0, sizeof(RRM_STATS_INFO_STRU));

    return;
}



VOS_VOID  RRM_ShowStatsInfo(VOS_VOID)
{
    VOS_UINT16                          usIndex;


    for (usIndex = 0; usIndex < MODEM_ID_BUTT; usIndex++)
    {
        vos_printf("========MODEM Msg info for Index %d ========\n", usIndex);
        vos_printf("申请RFID的次数:          %d \n", g_stRrmStats.astRrmModemStats[usIndex].ulRrmRcvResApplyNum);
        vos_printf("回复RFID申请的次数:      %d \n", g_stRrmStats.astRrmModemStats[usIndex].ulRrmSndApplyCnfNum);
        vos_printf("释放RFID的次数:          %d \n", g_stRrmStats.astRrmModemStats[usIndex].ulRrmRcvResReleaseNum);
        vos_printf("注册的次数:              %d \n", g_stRrmStats.astRrmModemStats[usIndex].ulRrmRcvRegNum);
        vos_printf("去注册的次数:            %d \n", g_stRrmStats.astRrmModemStats[usIndex].ulRrmRcvDeregNum);
    }
    vos_printf("\r\n");
    vos_printf("发送STATUS IND的次数:        %d \n", g_stRrmStats.ulRrmSndStatusIndNum);
    vos_printf("\r\n");

    return;

}



VOS_VOID RRM_Help(VOS_VOID)
{

    vos_printf("********************RRM软调信息************************\n");
    vos_printf("RRM_ShowInfo          显示RRM模块当前信息\n");
    vos_printf("RRM_ShowStatsInfo     显示RRM模块统计信息\n");

    return;
}


#endif /* FEATURE_ON == FEATURE_DSDS */


#ifdef __cplusplus
    #if __cplusplus
        }
    #endif
#endif

