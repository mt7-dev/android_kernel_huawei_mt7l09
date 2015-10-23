/*lint --e{537} */
#include <hi_onoff.h>
#include <semLib.h>
#include <drv_onoff.h>
#include <power_com.h>
#include <bsp_sram.h>
#include <bsp_om.h>
#include <bsp_icc.h>
#include <bsp_onoff.h>
#include <osl_types.h>
#include <osl_wait.h>
#include <osl_sem.h>
#include <hi_syssc_interface.h>

static stPowCtrl g_pwrctrl;

static osl_sem_id g_sem_mode;
static int g_start_mode = (int)DRV_START_MODE_BUILT;


#ifndef HI_ONOFF_PHONE
/*****************************************************************************
 函 数 名  : power_off_reboot_operation
 功能描述  : pmu rst 系统重启
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
static void power_off_reboot_operation( void )
{
/* p531 FPGA复位需要同时复位F1/BBP/RF */
#ifdef BSP_CONFIG_P531_FPGA
extern void p531_fpga_reset(void);
	p531_fpga_reset();
#endif

    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF,"[onoff]system power reboot!\n");

	hi_syssc_pmu_reboot();

    /* coverity[no_escape] */
    for(;;) ;
}

#endif

/******************************************************************************
*  Function:  bsp_drv_power_reboot_direct
*  Description:
*  Input:
*         None
*  Output:
*         None
*  Return:
*         None
*  Note  : 直接重启
********************************************************************************/
void bsp_drv_power_reboot_direct( void )
{
#ifndef HI_ONOFF_PHONE
    power_off_reboot_operation();
#else
    bsp_drv_power_reboot();
#endif
}

/*****************************************************************************
 函 数 名  : power_on_c_status_set
 功能描述  : 清空C核状态
 输入参数  : power_on_c_status_set
 输出参数  : 无
 返 回 值  : 无
 调用函数  :
 被调函数  :
*****************************************************************************/
void power_on_c_status_set(c_power_st_e val)
{
    power_info_s *pw_st = (power_info_s *)(SRAM_REBOOT_ADDR);

    pw_st->c_power_state = (u32)val;
}

/*****************************************************************************
 函 数 名  : bsp_start_mode_get
 功能描述  : 用于C核获取开机模式,通过核间调用, 启动的时候不允许获取
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int  bsp_start_mode_get( void )
{
    int rt = 0;
    u32 channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_ONOFF;
    DRV_START_MODE start_mode = DRV_START_MODE_NORMAL;
    stCtrlMsg msg = g_pwrctrl.curr;

    if(!g_pwrctrl.is_init)
        return start_mode;

    if (g_start_mode != (int)DRV_START_MODE_BUILT)
        return g_start_mode;

    msg.pwr_type = E_POWER_ON_MODE_GET;
    msg.mode = DRV_START_MODE_NORMAL;

    osl_sem_down(&(g_pwrctrl.pwr_sem));

    rt = bsp_icc_send(ICC_CPU_APP, channel_id, (u8*)&msg,
        sizeof(stCtrlMsg)); //lint !e40
    if(rt != (int)sizeof(stCtrlMsg))
    {
        osl_sem_up(&(g_pwrctrl.pwr_sem));
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF,"[onoff]onoff icc send error, rt=0x%x\n", rt);
        return start_mode;
    }

    /*coverity[lock] */
    osl_sem_down(&g_sem_mode);

    start_mode = (DRV_START_MODE)g_start_mode;

    osl_sem_up(&(g_pwrctrl.pwr_sem));

    /*coverity[missing_unlock] */
    return start_mode;
}

/*****************************************************************************
 函 数 名  : drv_shut_down
 功能描述  : 用于C核进行关机处理,通过核间调用
 输入参数  : 关机原因
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void drv_shut_down( DRV_SHUTDOWN_REASON_ENUM enReason )
{
    u32 channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_ONOFF;
    stCtrlMsg msg = g_pwrctrl.curr;
    int rt = 0;

    msg.pwr_type = E_POWER_SHUT_DOWN;
    msg.reason = enReason;

    if(!g_pwrctrl.is_init)
        return;

    osl_sem_down(&(g_pwrctrl.pwr_sem));
    if(sizeof(stCtrlMsg) != (u32)(rt = bsp_icc_send(ICC_CPU_APP, channel_id, (u8*)&msg, sizeof(stCtrlMsg))))
    {
        osl_sem_up(&(g_pwrctrl.pwr_sem));
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF,"[onoff]c core excute shut down failed rt=0x%x \n", rt);
        return ;
    }

    osl_sem_up(&(g_pwrctrl.pwr_sem));
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF,"[onoff]ccore shut down cmd send success,waiting for system shut down\n");

    return ;
}

/*****************************************************************************
 函 数 名  : drv_shut_down
 功能描述  : 用于C核进行关机处理,通过核间调用
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_drv_power_off( void )
{
    u32 channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_ONOFF;
    stCtrlMsg msg = g_pwrctrl.curr;
    int rt = 0;

    msg.pwr_type = E_POWER_POWER_OFF;

    if(!g_pwrctrl.is_init)
        return;

    osl_sem_down(&(g_pwrctrl.pwr_sem));

    if(sizeof(stCtrlMsg) != (u32)(rt = bsp_icc_send(ICC_CPU_APP, channel_id, (u8*)&msg, sizeof(stCtrlMsg))))
    {
        osl_sem_up(&(g_pwrctrl.pwr_sem));
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF,"[onoff]c core excute poweroff failed rt=0x%x \n", rt);
        return ;
    }

    osl_sem_up(&(g_pwrctrl.pwr_sem));
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF,"[onoff]ccore power off cmd send success,waiting for system power off\n");

    return ;
}

/*****************************************************************************
 函 数 名  : bsp_drv_power_reboot
 功能描述  : 用于C核进行重启处理,通过核间调用
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
void bsp_drv_power_reboot( void )
{
    u32 channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_ONOFF;
    stCtrlMsg msg = g_pwrctrl.curr;
    int rt = 0;

    msg.pwr_type = E_POWER_POWER_REBOOT;

    if(!g_pwrctrl.is_init)
        return;

    osl_sem_down(&(g_pwrctrl.pwr_sem));

    if(sizeof(stCtrlMsg) != (u32)(rt = bsp_icc_send(ICC_CPU_APP, channel_id, (u8*)&msg, sizeof(stCtrlMsg))))
    {
        osl_sem_up(&(g_pwrctrl.pwr_sem));
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF,"[onoff]c core excute power reboot failed rt=0x%x \n", rt);
        return ;
    }

    osl_sem_up(&(g_pwrctrl.pwr_sem));
    bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF,"[onoff]ccore power reboot cmd send success,waiting for system power reboot\n");

    return ;
}

/*****************************************************************************
 函 数 名  : bsp_on_off_icc_callback
 功能描述  : C核开关机ICC回调函数
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
/*lint --e{715 } */
static int bsp_on_off_icc_callback(u32 icc_channel_id , u32 len, void* context)
{
    int ret = 0;
    int mode = 0;

    ret = bsp_icc_read(icc_channel_id, (u8*)&mode, len);
    if (ret != (int)sizeof(mode))
    {
        mode = (int)DRV_START_MODE_NORMAL;
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF, "on off icc read error, return length:%x", ret);
    }
    g_start_mode = mode;

    osl_sem_up(&g_sem_mode);

    return 0;
}

/*****************************************************************************
 函 数 名  : bsp_on_off_init
 功能描述  : C核开关机初始化
 输入参数  :
 输出参数  : 无
 返 回 值  :
 调用函数  :
 被调函数  :
*****************************************************************************/
int bsp_on_off_init( void )
{
    int ret = 0;
    u32 icc_channel_id = ICC_CHN_IFC << 16 | IFC_RECV_FUNC_ONOFF;

    memset((void*)&g_pwrctrl,0,sizeof(g_pwrctrl));
    osl_sem_init(1, &(g_pwrctrl.pwr_sem));

    osl_sem_init(0, &g_sem_mode);

    ret |= bsp_icc_event_register(icc_channel_id,
        (read_cb_func)bsp_on_off_icc_callback, NULL, NULL, NULL);
    if (ret)
    {
        bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ONOFF, "onoff init error, error code: 0x%x\r\n", ret);
    }

    g_pwrctrl.is_init = 1;

    return ret;
}

