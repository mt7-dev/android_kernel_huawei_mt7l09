

/*lint --e{537}*/
/*system*/
#include <vxWorks.h>
#include <string.h>
#include <logLib.h>
#include <intLib.h>
#include <stdlib.h>
#include <cacheLib.h>
/*product*/
#include "product_config.h"
/*osl*/
#include <osl_spinlock.h>
/*include/platform*/
#include <drv_nv_id.h>
#include <drv_nv_def.h>
#include <drv_timer.h>

#include "soc_memmap.h"
#include "hi_base.h"
#include "hi_syssc.h"
#include "hi_syssc_interface.h"
#include "hi_syscrg.h"
#include "hi_syscrg_interface.h"
#include "hi_pwrctrl.h"
#include "hi_pwrctrl_interface.h"

#include "hi_bbp_ctu.h"
#include "hi_bbp_int.h"
#include "hi_bbp_stu.h"
#include "hi_bbp_systime.h"
#include "hi_bbp_tstu.h"
#include "hi_bbp_ltedrx.h"
#include "hi_bbp_tdsdrx.h"
#include "hi_bbp_ul.h"
#include "hi_bbp.h"

/*others*/
#include "bsp_memmap.h"
#include <bsp_version.h>
#include <bsp_memrepair.h>
#include <bsp_nvim.h>
#include <bsp_hw_spinlock.h>
#include <bsp_dump.h>
#include "bsp_dpm.h"
#include "bsp_ipc.h"
#include <bsp_hardtimer.h>
#include "ios_balong.h"
/*self*/
#include "bsp_bbp.h"


typedef struct tagBBP_DEBUG_INFO_S
{
    u32 bbpdltbcnt;
    u32 bbptimercnt;
    u32 bbptdscnt;
    BSPBBPIntDlTbFunc  bbpdltbfunc;
    BSPBBPIntTimerFunc bbptimerfunc;
    BSPBBPIntTdsFunc   g_BBPIntTdsTFFunc;
}BBP_INFO_S;

typedef enum{
	BBP_IRM = 0,
	BBP_G1,
	BBP_G2,
	BBP_TW,
	BBP_W,
	BBP_L,
	BBP_BUTT
}BBP_MODE_ID_E;

typedef struct
{
    u32 reg_start;/*需要保存的寄存器片断的起始地址*/
    u32 size;    /* 需要存储的连续寄存器空间大小，单位为byte */
}BBP_BBC_REG_STRU;

#define BBP_POWER_CTRL_VOTE_INIT  (unsigned char)(0<<PWC_COMM_MODE_WCDMA \
												| 0<<PWC_COMM_MODE_GSM \
												| 0<<PWC_COMM_MODE_LTE \
												| 0<<PWC_COMM_MODE_TDS)
#define BBP_CTU_DMA_CHN 8 /* 随便用通道号，和DSP不冲突*/
#define BBP_CTU_DATA_SIZE (unsigned int)0x1000			/*确认这里是否可以写成0x1000*/
#define BBP_DMA_SAVE_S_OFFSET (0x0a00+0x10*BBP_CTU_DMA_CHN)/*数据保存的目地址*/
#define BBP_DMA_SAVE_D_OFFSET (0x0a04+0x10*BBP_CTU_DMA_CHN)/*数据保存的源地址*/
#define BBP_DMA_STATUS_OFFSET (0x0a08+0x10*BBP_CTU_DMA_CHN)/*数据状态寄存器*/
#define BBP_DMA_CONFIG_OFFSET (0x0a0c+0x10*BBP_CTU_DMA_CHN)/*配置搬移数据长度、模式、使能*/

#define BBP_DMA_LEN_BIT_OFFSET	0
#define BBP_DMA_FAST_BIT_OFFSET	16
#define BBP_DMA_STATUS_BIT_OFFSET 16
#define BBP_DMA_ALL_DATA_BIT_OFFSET 20
#define BBP_DMA_FAST_SOFT_BIT_OFFSET 24
#define BBP_DMA_ENABLE_BIT_OFFSET 28

#define BBP_IRM_SAVE_OFFSET 0xa000

#define WAKEUP_INT_CLEAR_TIMEOUT 8

#define BBC_SAVE_DMA/*控制保存方式的宏*/

BBP_INFO_S g_stbbpinfo = {0};
spinlock_t bbp_power_ctrl[BBP_BUTT];
u32 chip_version;/*for bbp on read back*/

unsigned int bsp_bbp_pwrctrl_tdsbbp_clk_getstatus(void);

int bsp_bbp_int_debug(void)
{
	bbp_print_error("bbp lte timer int Num : %d\n",INT_LVL_BBPTIMER);
	//bbp_print_error("bbp lte timer int arm status:%d\n",);/*查看arm使能状态*/
	bbp_print_error("bbp lte timer int bbp status: %d\n",get_hi_bbp_int_arm_int_msk_arm_position_int_msk());
	bbp_print_error("bbp lte timer int count: %d\n",g_stbbpinfo.bbptimercnt);

	bbp_print_error("bbp tds timer int Num : %d\n",INT_LVL_TDSSTU);
	//bbp_print_error("bbp tds timer int arm status:%d\n",);/*查看arm使能状态*/
	bbp_print_error("bbp tds timer int bbp status: %d\n",get_hi_bbp_tstu_pub_int_msk_arm_sfrm_int_msk());
	bbp_print_error("bbp tds timer int count: %d\n",g_stbbpinfo.bbptdscnt);

	bbp_print_error("bbp dltb int Num : %d\n",INT_LVL_TDSSTU);
	//bbp_print_error("bbp dltb int arm status:%d\n",);/*查看arm使能状态*/
	bbp_print_error("bbp dltb int bbp status: %d\n",get_hi_bbp_tstu_pub_int_msk_arm_sfrm_int_msk());
	bbp_print_error("bbp dltb int count: %d\n",g_stbbpinfo.bbptdscnt);

	return 0;
}
/*****************************************************************************
* 函数  : bsp_bbp_get_twmtcmos_status
* 功能  : 本模块内部使用，用作对tds寄存器访问掉电保护
* 输入  : pfunc
* 输出  : void
* 返回  : void
*****************************************************************************/
#ifdef BSP_CONFIG_HI3630
	
#define BBP_INT_COUNT 100

u32 * TdsIntRegLog;
u32 * TdsIntSliceLog;

u32 TdsIntLogCnt = 0;

static unsigned int bsp_bbp_get_twmtcmos_status(void)
{
    unsigned int ret	= 0;
	unsigned int temp	= 0;
	u32 board_type = bsp_version_get_board_chip_type();

	/*add log*/
	TdsIntLogCnt++;
	if (TdsIntLogCnt >=BBP_INT_COUNT)
		TdsIntLogCnt = 0;
	TdsIntSliceLog[TdsIntLogCnt] = bsp_get_slice_value();

	if(HW_VER_K3V3_FPGA == board_type || HW_VER_PRODUCT_P531_FPGA == board_type){
		TdsIntRegLog[TdsIntLogCnt]= 0x5a5a5a5a;
		return 1;
	}
	else{
             ret = get_hi_pwr_stat4_twbbp_mtcmos_ctrl();
			 TdsIntRegLog[TdsIntLogCnt]= ret;

			 temp = get_hi_pwr_stat1_twbbp_mtcmos_rdy();
             ret &= temp;
			 TdsIntRegLog[TdsIntLogCnt]|= temp<<8;
			 
             temp = bsp_bbp_pwrctrl_tdsbbp_clk_getstatus();
			 ret &= temp;
			 TdsIntRegLog[TdsIntLogCnt]|= temp<<16;

             temp = get_hi_pwr_stat1_twbbp_mtcmos_rdy();
			 ret &= temp;
			 TdsIntRegLog[TdsIntLogCnt]|= temp<<24;

		    return ret;
	}
}
#else
static unsigned int bsp_bbp_get_twmtcmos_status(void)
{
    unsigned int ret =0;
	
	u32 board_type = bsp_version_get_board_chip_type();


	if(HW_VER_K3V3_FPGA == board_type || HW_VER_PRODUCT_P531_FPGA == board_type){
		return 1;
	}
	else{
             ret = get_hi_pwr_stat4_twbbp_mtcmos_ctrl();
             ret &= get_hi_pwr_stat1_twbbp_mtcmos_rdy();
             ret &=bsp_bbp_pwrctrl_tdsbbp_clk_getstatus();
             ret &= get_hi_pwr_stat1_twbbp_mtcmos_rdy();
		    return ret;
	}
}

#endif

/*****************************************************************************
* 函数  : bsp_bbp_get_ltemtcmos_status
* 功能  : 本模块内部使用，用作对lte寄存器访问掉电保护
* 输入  : pfunc
* 输出  : void
* 返回  : void
*****************************************************************************/
static unsigned int bsp_bbp_get_ltemtcmos_status(void)
{
	u32 board_type = bsp_version_get_board_chip_type();

	if(HW_VER_K3V3_FPGA == board_type || HW_VER_PRODUCT_P531_FPGA == board_type){
		return 1;
	}
	else{
		return (get_hi_pwr_stat4_lbbp_mtcmos_ctrl()& \
			    get_hi_pwr_stat1_lbbp_mtcmos_rdy() & \
			    get_hi_pwr_stat1_ltebbp0_mtcmos_rdy_stat());
	}
}
/*****************************************************************************
* 函数  : bsp_bbp_timerintregcb
* 功能  : 被PS调用，用来向底软注册1ms定时中断的回调
* 输入  : pfunc
* 输出  : void
* 返回  : void
*****************************************************************************/
void bsp_bbp_timerintregcb(BSPBBPIntTimerFunc pfunc)
{
    g_stbbpinfo.bbptimerfunc = pfunc;
}

/*****************************************************************************
* 函数  : bsp_bbp_dltbintregcb
* 功能  : 被PS调用，用来向底软注册下行数据译码完成中断的回调
* 输入  : pfunc
* 输出  : void
* 返回  : void
*****************************************************************************/
void bsp_bbp_dltbintregcb(BSPBBPIntDlTbFunc pfunc)
{
#if 0
	/*V7R2及后续芯片中，DL_DMA中断不在使用，
	   为了上层在V9R1与V7R2代码统一，此处提供桩接口即可*/

    g_stbbpinfo.bbpdltbfunc = pfunc;
#endif
}

/*****************************************************************************
* 函 数: bsp_bbp_timerintclear
* 功 能: 被PS调用，用来清除1ms定时中断
* 输入 : void
* 输出 : void
* 返 回: void
*****************************************************************************/
void bsp_bbp_timerintclear(void)
{
	unsigned long flag;
	u32 lbbp_switch = 0;

    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	lbbp_switch = bsp_bbp_get_ltemtcmos_status();
	if(lbbp_switch){
	    set_hi_bbp_int_arm_int_cls_arm_position_int_cls(0x1);
	}
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
}

/*****************************************************************************
* 函 数: bsp_bbp_timerintenable
* 功 能: 被PS调用，用来打开1ms定时中断
* 输入 : void
* 输出 : void
* 返 回: void
*****************************************************************************/
int bsp_bbp_timerintenable(void)
{
	unsigned long flag;
	u32 lbbp_switch = 0;

    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	lbbp_switch = bsp_bbp_get_ltemtcmos_status();
	if(lbbp_switch){
		set_hi_bbp_int_arm_int_cls_arm_position_int_cls(0x1);
	    set_hi_bbp_int_arm_int_msk_arm_position_int_msk(0x1);
	    intEnable(INT_LVL_BBPTIMER);
	}
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
    return BSP_OK;
}

/*****************************************************************************
* 函 数: bsp_bbp_timerintdisable
* 功 能: 被PS调用，用来关闭1ms定时中断
* 输入 : void
* 输出 : void
* 返 回: void
*****************************************************************************/
void bsp_bbp_timerintdisable(void)
{
	unsigned long flag;
	u32 lbbp_switch = 0;

    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	lbbp_switch = bsp_bbp_get_ltemtcmos_status();
	if(lbbp_switch){
	    set_hi_bbp_int_arm_int_msk_arm_position_int_msk(0x0);
	}
	intDisable(INT_LVL_BBPTIMER);
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
    return ;
}


/*****************************************************************************
* 函数  : bsp_bbp_timerhandle
* 功能  : 1ms中断处理函数
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
static void bbp_timerhandle(void)
{
	unsigned long flag;
	u32 lbbp_switch = 0;

    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	lbbp_switch = bsp_bbp_get_ltemtcmos_status();
	if(lbbp_switch){
	    set_hi_bbp_int_arm_int_cls_arm_position_int_cls(0x1);
    	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

    	g_stbbpinfo.bbptimercnt++;

		if (g_stbbpinfo.bbptimerfunc)
		{
			(g_stbbpinfo.bbptimerfunc)();
		}
	}
	else{
    	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
	}
	return ;

}

/*****************************************************************************
* 函数  : bsp_bbp_dltbhandle
* 功能  : 下行数据译码完成中断处理函数
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
static void bbp_dltbhandle(void)
{
#if 0
	/*V7R2及后续芯片中，DL_DMA中断不在使用，
	   为了上层在V9R1与V7R2代码统一，此处提供桩接口即可*/

	unsigned long flag;
	u32 lbbp_switch = 0;

    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	lbbp_switch = bsp_bbp_get_ltemtcmos_status();
	if(lbbp_switch){
	    set_hi_bbp_int_arm_int_cls_arm_dl_dma_int_cls(0x1);

		bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

	    g_stbbpinfo.bbpdltbcnt++;

	    if (g_stbbpinfo.bbpdltbfunc)
	    {
	        (g_stbbpinfo.bbpdltbfunc)();
	    }
	}
	else{
    	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
	}
#endif
}


/*****************************************************************************
* 函数  : bsp_bbp_getcurtime
* 功能  : 被PS调用，用来获取系统精确时间
* 输入  : void
* 输出  : u64 *pcurtime
* 返回  : u32
*****************************************************************************/
u32 bsp_bbp_getcurtime(unsigned long long *pcurtime)
{
	/*lint -save -e958*/
    unsigned long long timervalue[4];
	/*lint -save -restore*/

    timervalue[0] = get_hi_bbp_systime_abs_timer_l_abs_timer_l();
    timervalue[1] = get_hi_bbp_systime_abs_timer_h_abs_timer_h();
    timervalue[2] = get_hi_bbp_systime_abs_timer_l_abs_timer_l();
    timervalue[3] = get_hi_bbp_systime_abs_timer_h_abs_timer_h();

    if(timervalue[2] < timervalue[0])
    {
        (*pcurtime) = ((timervalue[3] - 1) << 32) | timervalue[0];
    }
    else
    {
        (*pcurtime) = (timervalue[1] << 32) | timervalue[0];
    }

    return 0;
}

/*****************************************************************************
* 函数  : bsp_bbp_getsysframe
* 功能  : get system frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
u16 bsp_bbp_getsysframe(void)
{
	unsigned long flag;
	u32 lbbp_switch = 0;
    u16 framenum = BBP_POWER_DOWN_BACK;

    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	lbbp_switch = bsp_bbp_get_ltemtcmos_status();
	if(lbbp_switch){
		set_hi_bbp_stu_tim_lock_tim_lock(0x1);
		framenum = (u16)get_hi_bbp_stu_sfn_rpt_sfn_rpt();
	}
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

    return framenum;
}

/*****************************************************************************
* 函数  : bsp_bbp_getsyssubframe
* 功能  : get system sub frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
u16 bsp_bbp_getsyssubframe(void)
{
    u16 framenum = BBP_POWER_DOWN_BACK;
	unsigned long flag;
	u32 lbbp_switch = 0;

    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	lbbp_switch = bsp_bbp_get_ltemtcmos_status();
	if(lbbp_switch){
	    set_hi_bbp_stu_tim_lock_tim_lock(0x1);
	    framenum = (u16)get_hi_bbp_stu_subfrm_num_rpt_subfrm_num_rpt();
	}
    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

    return framenum;
}
/*****************************************************************************
* 函数  : bsp_bbp_get_tds_subframe
* 功能  : get tds system sub frame num
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
u16 bsp_bbp_get_tds_subframe(void)
{
    u16 framenum = BBP_POWER_DOWN_BACK;
	unsigned long flag;
	u32 lbbp_switch = 0;

    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	lbbp_switch = bsp_bbp_get_twmtcmos_status();
	if(lbbp_switch){
		/*TODO:和liqingbo确认，是否需要锁存器*/
		framenum = (u16)get_hi_bbp_tstu_tds_stu_sfn_tds_stu_sfn();
		framenum &= BBP_TDS_STU_SFN_MASK;
	}
	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

    return framenum;
}

/*****************************************************************************
* 函数  : bsp_bbp_set_tds_subframoffset
* 功能  : set tds system sub frame offset
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
u16 bsp_bbp_set_tds_subframoffset(u16 sf_offset)
{
	u16 ret = BBP_POWER_DOWN_BACK;
	unsigned long flag;
	u32 lbbp_switch = 0;

	bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	lbbp_switch = bsp_bbp_get_twmtcmos_status();
	if(lbbp_switch){
		/*配置子帧偏移*/
		set_hi_bbp_tstu_sfn_oft_sfn_oft(sf_offset);
		/*定时切换使能，TODO:和sunli确认，此处是否需要*/
		set_hi_bbp_tstu_time_adj_start_time_switch_start(1);
		ret = BSP_OK;
	}
	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

	return ret;

}


/*****************************************************************************
* 函数  : bsp_bbp_tdstf_regcb
* 功能  : regist callback
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/

void bsp_bbp_tdstf_regcb(BSPBBPIntTdsFunc pFunc)
{
    if(NULL != pFunc)
    {
        g_stbbpinfo.g_BBPIntTdsTFFunc = pFunc;
    }
    else
    {
        logMsg("Para pFunc is NULL,Wrong Para:%d\n", __LINE__, 0, 0, 0, 0, 0);
    }
}

/*****************************************************************************
* 函数  : bsp_bbp_tdstf_enable
* 功能  : enable tdstf
* 输入  : void
* 输出  : viod
* 返回  : u32
*****************************************************************************/

int bsp_bbp_tdstf_enable(void)
{
	u32 tbbp_switch = 0;
	unsigned long flag;
    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);
	intEnable(INT_LVL_TDSSTU);

	/*掉电保护，如果BBP掉电，返回-1*/
	tbbp_switch = bsp_bbp_get_twmtcmos_status();
	if(tbbp_switch){

	    //set_hi_bbp_tdsint_pub_int_msk_arm_sfrm_int_msk(0x1);
	    set_hi_bbp_tstu_pub_int_msk_arm_sfrm_int_msk(0x1);

    	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
		return BSP_OK;
	}

    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
    return BSP_ERROR;
}

/*****************************************************************************
* 函数  : bsp_bbp_tdstf_disable
* 功能  : disable tdstf
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/

void bsp_bbp_tdstf_disable(void)
{
	u32 tbbp_switch = 0;
	unsigned long flag;
    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);
	intDisable(INT_LVL_TDSSTU);
	/*掉电保护，如果BBP掉电，返回*/
	tbbp_switch = bsp_bbp_get_twmtcmos_status();
	if(tbbp_switch){

	    //set_hi_bbp_tdsint_pub_int_msk_arm_sfrm_int_msk(0x0);
	    set_hi_bbp_tstu_pub_int_msk_arm_sfrm_int_msk(0x0);
	}

    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
    return;
}

/*****************************************************************************
* 函数  : bsp_bbp_tdstf_clear
* 功能  : clear tdstf int
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/
static void bbp_tdstf_clear(void)
{
	u32 tbbp_switch = 0;
	unsigned long flag;

    bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	/*掉电保护，如果BBP掉电，返回0xffff*/
	tbbp_switch = bsp_bbp_get_twmtcmos_status();
	if(tbbp_switch){

	    //set_hi_bbp_tdsint_pub_int_clear_arm_sfrm_int_clear(0x1);
	    set_hi_bbp_tstu_pub_int_clear_arm_sfrm_int_clear(0x1);
	}

    bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
    return;
}

/*****************************************************************************
* 函数  : bsp_bbp_tdstf_handle
* 功能  : tdstf int handler
* 输入  : void
* 输出  : void
* 返回  : void
*****************************************************************************/

static void bbp_tdstf_handle(void)
{
    bbp_tdstf_clear();  //clear int

    (g_stbbpinfo.bbptdscnt)++;

    if(g_stbbpinfo.g_BBPIntTdsTFFunc != NULL)
    {
        (g_stbbpinfo.g_BBPIntTdsTFFunc)();
    }
}

int bbp_pwrctrl_irm_poweron(void)
{
    u32 value = 0;
	int ret = BSP_OK;
	unsigned long flag;
	u32 board_type;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_IRM], flag);

	board_type = bsp_version_get_board_chip_type();

	if(HW_VER_K3V3_FPGA == board_type || HW_VER_PRODUCT_P531_FPGA == board_type){
		goto out;
	}

	/*若已打开，无需打开*/
	if(bbp_pwrctrl_irm_status() == PWC_COMM_MODEM_ON){
		ret = BSP_OK;
		goto out;
	}

    /*1.配置IRM 复位*/
    set_hi_crg_srsten3_irm_pd_srst_en(0x1);

    /*2.配置IRM 上电*/
    set_hi_pwr_ctrl6_irm_mtcmos_en(0x1);

    /*等待上电完成*/
    do
    {
        value = get_hi_pwr_stat1_irm_mtcmos_rdy();
    }
    while(value==0);

	udelay(30);

    /*12. 配置IRM 解复位*/
    set_hi_crg_srstdis3_irm_pd_srst_dis(0x1);

	/*13. 配置IRM 的嵌位不使能*/
	set_hi_pwr_ctrl5_irm_iso_dis(0x1);

#ifdef MODEM_MEM_REPAIR
	
		ret = hwspin_lock_timeout(HW_MEMREPAIR_LOCKID,HW_SPINLOCK_WAIT_FOREVER);
		if(BSP_OK!=ret){
			bbp_print_error("hwspin lock timeout !\n");
		}
	
		ret = bsp_modem_memrepair(MODEM_MEMREPAIR_IRM);
		if(BSP_OK!=ret){
			bbp_print_error("memrepair fail !\n");
		}
	
		ret = hwspin_unlock(HW_MEMREPAIR_LOCKID);
		if(BSP_OK!=ret){
			bbp_print_error("hwspin unlock timeout !\n");
		}
#endif

	/*14. 配置IRM 的时钟使能*/
	set_hi_crg_clken5_irm_pd_clk_en(0x1);


out:
    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_IRM],flag);

    return ret;
}

int bbp_pwrctrl_irm_poweroff(void)
{
	int ret = BSP_OK;
	unsigned long flag;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_IRM], flag);

	/*若当前已关闭，则直接返回*/
	if(bbp_pwrctrl_irm_status() == PWC_COMM_MODEM_OFF){
		ret = BSP_OK;
		goto out;
	}

    /*1.配置IRM 的时钟不使能*/
    set_hi_crg_clkdis5_irm_pd_clk_dis(0x1);

    /*2.配置IRM 的嵌位使能*/
    set_hi_pwr_ctrl4_irm_iso_en(0x1);

    /*3.配置IRM 复位*/
    set_hi_crg_srsten3_irm_pd_srst_en(0x1);

    /*4.配置IRM 掉电*/
    set_hi_pwr_ctrl7_irm_mtcmos_dis(0x1);

out:
    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_IRM],flag);

    return ret;
}

PWC_COMM_STATUS_E bbp_pwrctrl_irm_status(void)
{
	u32 status=get_hi_pwr_stat4_irm_mtcmos_ctrl();

	if(status){
		return PWC_COMM_MODEM_ON;
	}
	else
		return PWC_COMM_MODEM_OFF;
}
/*****************************************************************************
* 函 数 : bsp_bbp_g1_poweron
* 功 能 : 被低功耗调用，用来给G1 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g1bbp_poweron(void)
{
    u32 value = 0;
	int ret = BSP_OK;
	unsigned long flag;
	u32 board_type;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_G1], flag);

	board_type = bsp_version_get_board_chip_type();

	if(HW_VER_K3V3_FPGA == board_type || HW_VER_PRODUCT_P531_FPGA == board_type){
		goto out;
	}

	/*若已打开，无需打开*/
	if(bsp_bbp_pwrctrl_g1bbp_status() == PWC_COMM_MODEM_ON){
		ret = BSP_OK;
		goto out;
	}

	/*1.配置G1BBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<3)*/
	set_hi_crg_srsten3_g1bbp_pd_srst_en(1);

	/*2.配置G1BBP上电(配置寄存器PWR_CTRL6(0x90000c18)=(0x1<<9))*/
	set_hi_pwr_ctrl6_g1bbp_mtcmos_en(1);

    /*等待上电完成*/
    do
    {
        value = get_hi_pwr_stat1_g1bbp_mtcmos_rdy();
    }
    while(value==0);

	udelay(30);
	/*3.配置G1BBP的解复位(配置寄存器CRG_SRSTDIS3(0x9000007C)=(0x1<<3)*/
	set_hi_crg_srstdis3_g1bbp_pd_srst_dis(1);

	/*4.配置G1BBP的嵌位不使能(配置寄存器PWR_CTRL5(0x90000c14)=(0x1<<9))*/
	set_hi_pwr_ctrl5_g1bbp_iso_dis(1);

	/*5.配置G1BBP的时钟使能(配置寄存器CRG_CLKEN5(地址0x90000030)=(0x1<<3)*/
	set_hi_crg_clken5_g1bbp_pd_clk_en(1);

out:

    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_G1],flag);

	return ret;
}

/*****************************************************************************
* 函 数 : bsp_bbp_g1_poweroff
* 功 能 : 被低功耗调用，用来给G1 BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g1bbp_poweroff(void)
{
	int ret = BSP_OK;
	unsigned long flag;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_G1],flag);

	/*若已关闭，则返回*/
	if(bsp_bbp_pwrctrl_g1bbp_status() == PWC_COMM_MODEM_OFF){
		ret = BSP_OK;
		goto out;
	}

	/*配置G1BBP的时钟不使能(地址0x90000034)=(0x1<<3)*/
	set_hi_crg_clkdis5_g1bbp_pd_clk_dis(1);

	/*配置G1BBP的嵌位使能(配置寄存器PWR_CTRL4(0x90000c10)=(0x1<<9))*/
	set_hi_pwr_ctrl4_g1bbp_iso_en(1);

	/*配置G1BBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<3)*/
	set_hi_crg_srsten3_g1bbp_pd_srst_en(1);

	/*配置G1BBP掉电(配置寄存器PWR_CTRL7(0x90000c1c)=(0x1<<9))*/
	set_hi_pwr_ctrl7_g1bbp_mtcmos_dis(1);

out:
    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_G1],flag);

	return ret;

}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_g1_status
* 功 能 : 被低功耗调用，用来给G1 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_g1bbp_status(void)
{
	u32 status = (get_hi_pwr_stat4_g1bbp_mtcmos_ctrl() &
		          get_hi_pwr_stat1_g1bbp_mtcmos_rdy());

	if(status){
		return PWC_COMM_MODEM_ON;
	}
	else
		return PWC_COMM_MODEM_OFF;
}

/*****************************************************************************
* 函 数 : bsp_bbp_g2_poweron
* 功 能 : 被低功耗调用，用来给G2 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g2bbp_poweron(void)
{
    u32 value = 0;
	int ret = BSP_OK;
	unsigned long flag;
	u32 board_type;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_G2],flag);

	board_type = bsp_version_get_board_chip_type();

	if(HW_VER_K3V3_FPGA == board_type || HW_VER_PRODUCT_P531_FPGA == board_type){
		goto out;
	}

	/*若已关闭，则返回*/
	if(bsp_bbp_pwrctrl_g2bbp_status() == PWC_COMM_MODEM_ON){
		ret = BSP_OK;
		goto out;
	}

	/*配置G2BBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<4)*/
	set_hi_crg_srsten3_g2bbp_pd_srst_en(1);

	/*配置G2BBP上电(配置寄存器PWR_CTRL6(0x90000c18)=(0x1<<10))*/
	set_hi_pwr_ctrl6_g2bbp_mtcmos_en(1);

    /*等待上电完成*/
    do
    {
        value = get_hi_pwr_stat1_g2bbp_mtcmos_rdy();
    }
    while(value==0);

	udelay(30);

	/*配置G2BBP的解复位(配置寄存器CRG_SRSTDIS3(0x9000007C)=(0x1<<4)*/
	set_hi_crg_srstdis3_g2bbp_pd_srst_dis(1);

	/*配置G1BBP的嵌位不使能(配置寄存器PWR_CTRL5(0x90000c14)=(0x1<<10))*/
	set_hi_pwr_ctrl5_g2bbp_iso_dis(1);

	/*配置G2BBP的时钟使能(配置寄存器CRG_CLKEN5(地址0x90000030)=(0x1<<4)*/
	set_hi_crg_clken5_g2bbp_pd_clk_en(1);

out:
    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_G2],flag);

	return ret;
}

/*****************************************************************************
* 函 数 : bsp_bbp_g2_poweroff
* 功 能 : 被低功耗调用，用来给G1 BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_g2bbp_poweroff(void)
{
	int ret = BSP_OK;

	unsigned long flag;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_G2],flag);

	/*若已关闭，则返回*/
	if(bsp_bbp_pwrctrl_g2bbp_status() == PWC_COMM_MODEM_OFF){
		ret = BSP_OK;
		goto out;
	}

	/*配置G2BBP的时钟不使能(地址0x90000034)=(0x1<<4)*/
	set_hi_crg_clkdis5_g2bbp_pd_clk_dis(1);

	/*配置G2BBP的嵌位使能(配置寄存器PWR_CTRL4(0x90000c10)=(0x1<<10))*/
	set_hi_pwr_ctrl4_g2bbp_iso_en(1);

	/*配置G2BBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<4)*/
	set_hi_crg_srsten3_g2bbp_pd_srst_en(1);

	/*配置G2BBP掉电(配置寄存器PWR_CTRL7(0x90000c1c)=(0x1<<10))*/
	set_hi_pwr_ctrl7_g2bbp_mtcmos_dis(1);

out:
    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_G2],flag);

	return ret;

}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_g2_status
* 功 能 : 被低功耗调用，用来给G1 BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_g2bbp_status(void)
{
	u32 status = (get_hi_pwr_stat4_g2bbp_mtcmos_ctrl() & \
		          get_hi_pwr_stat1_g2bbp_mtcmos_rdy());

	if(status){
		return PWC_COMM_MODEM_ON;
	}
	else
		return PWC_COMM_MODEM_OFF;
}

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweron
* 功 能 : 被低功耗调用，用来给W BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_poweron(void)
{
    u32 value = 0;
	int ret = BSP_OK;
	unsigned long flag;
	u32 board_type;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_W],flag);

	board_type = bsp_version_get_board_chip_type();

	if(HW_VER_K3V3_FPGA == board_type || HW_VER_PRODUCT_P531_FPGA == board_type){
		goto out;
	}

	/*若已关闭，则返回*/
	if(bsp_bbp_pwrctrl_wbbp_status() == PWC_COMM_MODEM_ON){
		ret = BSP_OK;
		goto out;
	}

	/*配置WBBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<6)*/
	set_hi_crg_srsten3_wbbp_pd_srst_en(1);

	/*配置WBBP上电(配置寄存器PWR_CTRL6(0x90000c18)=(0x1<<8))*/
	set_hi_pwr_ctrl6_wbbp_mtcmos_en(1);

    /*等待上电完成*/
    do
    {
        value = get_hi_pwr_stat1_wbbp_mtcmos_rdy();
    }
    while(value==0);

	udelay(30);

	/*配置WBBP的解复位(配置寄存器CRG_SRSTDIS3(0x9000007C)=(0x1<<6)*/
	set_hi_crg_srstdis3_wbbp_pd_srst_dis(1);

	/*配置WBBP的嵌位不使能(配置寄存器PWR_CTRL5(0x90000c14)=(0x1<<8))*/
	set_hi_pwr_ctrl5_wbbp_iso_dis(1);

	/*配置WBBP的时钟使能(配置寄存器CRG_CLKEN5(地址0x90000030)=(0x1<<6)*/
	set_hi_crg_clken5_wbbp_pd_clk_en(1);

out:
    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_W],flag);

	return ret;
}

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweroff
* 功 能 : 被低功耗调用，用来给W BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_poweroff(void)
{
	int ret = BSP_OK;
	unsigned long flag;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_W],flag);

	/*若已关闭，则返回*/
	if(bsp_bbp_pwrctrl_wbbp_status()== PWC_COMM_MODEM_OFF){
		bbp_print_info("bbp wbbp has closed!\n");
		ret = BSP_OK;
		goto out;
	}

	/*配置WBBP的时钟不使能(地址0x90000034)=(0x1<<6)*/
	set_hi_crg_clkdis5_wbbp_pd_clk_dis(1);

	/*配置WBBP的嵌位使能(配置寄存器PWR_CTRL4(0x90000c10)=(0x1<<8))*/
	set_hi_pwr_ctrl4_wbbp_iso_en(1);

	/*配置WBBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<6)*/
	set_hi_crg_srsten3_wbbp_pd_srst_en(1);

	/*配置WBBP掉电(配置寄存器PWR_CTRL7(0x90000c1c)=(0x1<<8))*/
	set_hi_pwr_ctrl7_wbbp_mtcmos_dis(1);

out:
    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_W],flag);

	return ret;

}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_wbbp_status
* 功 能 : 被低功耗调用，用来给W BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_wbbp_status(void)
{
	u32 status = (get_hi_pwr_stat1_wbbp_mtcmos_rdy() & \
	              get_hi_pwr_stat4_wbbp_mtcmos_ctrl());

	if(status){
		return PWC_COMM_MODEM_ON;
	}
	else
		return PWC_COMM_MODEM_OFF;
}

/*****************************************************************************
* 函 数 : bsp_bbp_twbbp_poweron
* 功 能 : 被低功耗调用，用来给TW BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_twbbp_poweron(void)
{
    u32 value = 0;
	int ret = BSP_OK;
	unsigned long flag;
	u32 board_type;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_TW],flag);

	board_type = bsp_version_get_board_chip_type();

	if(HW_VER_K3V3_FPGA == board_type || HW_VER_PRODUCT_P531_FPGA == board_type){
		goto out;
	}

	/*若已打开，则返回*/
	if(bsp_bbp_pwrctrl_twbbp_status() == PWC_COMM_MODEM_ON){
		ret = BSP_OK;
		goto out;
	}

	/*配置TWBBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<5)*/
	set_hi_crg_srsten3_twbbp_pd_srst_en(1);

	/*配置TWBBP上电(配置寄存器PWR_CTRL6(0x90000c18)=(0x1<<7))*/
	set_hi_pwr_ctrl6_twbbp_mtcmos_en(1);


    /*等待上电完成*/
    do
    {
        value = get_hi_pwr_stat1_twbbp_mtcmos_rdy();
    }
    while(value==0);

	udelay(30);

	/*配置TWBBP的解复位(配置寄存器CRG_SRSTDIS3(0x9000007C)=(0x1<<5)*/
	set_hi_crg_srstdis3_twbbp_pd_srst_dis(1);

	/*配置TBBP的嵌位不使能(配置寄存器PWR_CTRL5(0x90000c14)=(0x1<<7))*/
	set_hi_pwr_ctrl5_twbbp_iso_dis(1);

	/*配置TWBBP的时钟使能(配置寄存器CRG_CLKEN5(地址0x90000030)=(0x1<<5)*/
	set_hi_crg_clken5_twbbp_pd_clk_en(1);

out:
    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_TW],flag);

	return ret;
}

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweroff
* 功 能 : 被低功耗调用，用来给W BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_twbbp_poweroff(void)
{
	int ret = BSP_OK;
	unsigned long flag;

    spin_lock_irqsave(&bbp_power_ctrl[BBP_TW],flag);

	/*若已关闭，则返回*/
	if(bsp_bbp_pwrctrl_twbbp_status()== PWC_COMM_MODEM_OFF){
		ret = BSP_OK;
		goto out;
	}

	/*配置G2BBP的时钟不使能(地址0x90000034)=(0x1<<5)*/
	set_hi_crg_clkdis5_twbbp_pd_clk_dis(1);

	/*配置G2BBP的嵌位使能(配置寄存器PWR_CTRL4(0x90000c10)=(0x1<<7))*/
	set_hi_pwr_ctrl4_twbbp_iso_en(1);

	/*配置G2BBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<5)*/
	set_hi_crg_srsten3_twbbp_pd_srst_en(1);

	/*配置G2BBP掉电(配置寄存器PWR_CTRL7(0x90000c1c)=(0x1<<7))*/
	set_hi_pwr_ctrl7_twbbp_mtcmos_dis(1);

out:
    spin_unlock_irqrestore(&bbp_power_ctrl[BBP_TW],flag);

	return ret;

}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_wbbp_status
* 功 能 : 被低功耗调用，用来给W BBP上电
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
PWC_COMM_STATUS_E bsp_bbp_pwrctrl_twbbp_status(void)
{
	u32 status = (get_hi_pwr_stat4_twbbp_mtcmos_ctrl() & \
		          get_hi_pwr_stat1_twbbp_mtcmos_rdy() );

	if(status){
		return PWC_COMM_MODEM_ON;
	}
	else
		return PWC_COMM_MODEM_OFF;
}
/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_ltebbp_status
* 功 能 : 被低功耗调用，用来查询L BBP上电情况
* 输 入 : void
* 输 出 : void
* 返 回 : 0x10为open, 0x20为close
* 说 明 :
*****************************************************************************/
static PWC_COMM_STATUS_E bsp_bbp_pwrctrl_ltebbp_status(void)
{
	u32 status = bsp_bbp_get_ltemtcmos_status();

	if(status){
		return PWC_COMM_MODEM_ON;
	}
	else
		return PWC_COMM_MODEM_OFF;
}

/*****************************************************************************
* 函 数 : bsp_bbp_wbbp_poweroff
* 功 能 : 被低功耗调用，用来给L BBP下电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
static int bsp_bbp_pwrctrl_ltebbp_poweroff(void)
{
	int ret = BSP_OK;
	unsigned long flag;

	spin_lock_irqsave(&bbp_power_ctrl[BBP_L],flag);

	/*若已关闭，则返回*/
	if(bsp_bbp_pwrctrl_ltebbp_status()== PWC_COMM_MODEM_OFF){
		ret = BSP_OK;
		goto out;
	}

	/*配置LTE BBP的时钟不使能(地址0x90000034)=(0x1<<2)*/
	set_hi_crg_clkdis5_ltebbp_pd_clk_dis(1);

	/*配置LTE BBP的嵌位使能(配置寄存器PWR_CTRL4(0x90000c10)=(0x1<<6))*/
	set_hi_pwr_ctrl4_lbbp_iso_en(1);

	/*配置LTE BBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<2)*/
	set_hi_crg_srsten3_lbbp_pd_srst_en(1);

	/*配置LTE BBP掉电(配置寄存器PWR_CTRL7(0x90000c1c)=(0x1<<6))*/
	set_hi_pwr_ctrl7_lbbp_mtcmos_dis(1);

out:
	spin_unlock_irqrestore(&bbp_power_ctrl[BBP_L],flag);

	return ret;
}


/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_ltebbp_poweron
* 功 能 : 被低功耗调用，用来给LTE BBP上电
* 输 入 : 模式
* 输 出 : void
* 返 回 : 下电是否成功
* 说 明 :
*****************************************************************************/
static int bsp_bbp_pwrctrl_ltebbp_poweron(void)
{
	/*V7r2底软不需要LBBP上电流程*/
    u32 value0 = 0;
	u32 value1 = 0;
	int ret = BSP_OK;
	unsigned long flag;
	u32 board_type;

	spin_lock_irqsave(&bbp_power_ctrl[BBP_L],flag);

	board_type = bsp_version_get_board_chip_type();

	if(HW_VER_K3V3_FPGA == board_type || HW_VER_PRODUCT_P531_FPGA == board_type){
		goto out;
	}

	/*若已打开，则返回*/
	if(bsp_bbp_pwrctrl_ltebbp_status() == PWC_COMM_MODEM_ON){
		ret = BSP_OK;
		goto out;
	}

	/*配置LTE BBP的复位(配置寄存器CRG_SRSTEN3(0x90000078)=(0x1<<2)*/
	set_hi_crg_srsten3_lbbp_pd_srst_en(1);

	/*配置LBBP上电(配置寄存器PWR_CTRL6(0x90000c18)=(0x1<<8))*/
	set_hi_pwr_ctrl6_lbbp_mtcmos_en(1);

	/*等待上电完成*/
	do
	{
		value0 = get_hi_pwr_stat1_lbbp_mtcmos_rdy();
		value1 = get_hi_pwr_stat1_ltebbp0_mtcmos_rdy_stat();
	}
	while((value0==0) || (value1==0));

	udelay(30);

	/*配置LBBP的解复位(配置寄存器CRG_SRSTDIS3(0x9000007C)=(0x1<<6)*/
	set_hi_crg_srstdis3_lbbp_pd_srst_dis(1);

	/*配置LBBP的嵌位不使能(配置寄存器PWR_CTRL5(0x90000c14)=(0x1<<8))*/
	set_hi_pwr_ctrl5_lbbp_iso_dis(1);

	/*配置LBBP的时钟使能(配置寄存器CRG_CLKEN5(地址0x90000030)=(0x1<<6)*/
	set_hi_crg_clken5_ltebbp_pd_clk_en(1);

out:
	spin_unlock_irqrestore(&bbp_power_ctrl[BBP_L],flag);

	return ret;
}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_tdsbbp_clk_enable
* 功 能 : 被低功耗调用，用来给TDS开钟
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_tdsbbp_clk_enable(void)
{
	set_hi_crg_clken5_twbbp_tbbp_122m_clk_en(1);
	set_hi_crg_clken5_twbbp_tbbp_245m_clk_en(1);

	return BSP_OK;
}
unsigned int bsp_bbp_pwrctrl_tdsbbp_clk_getstatus(void)
{
    unsigned int ret = 0;
	ret = get_hi_crg_clkstat5_twbbp_tbbp_122m_clk_stat();
	ret &= get_hi_crg_clkstat5_twbbp_tbbp_245m_clk_stat();

	return ret;
}


/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_tdsbbp_clk_disable
* 功 能 : 被低功耗调用，用来给TDS关钟
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_tdsbbp_clk_disable(void)
{
	set_hi_crg_clkdis5_pdtw_tbbp_122m_clk_dis(1);
	set_hi_crg_clkdis5_pdtw_tbbp_245m_clk_dis(1);

	return BSP_OK;
}
/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_wbbp_clk_disable
* 功 能 : 被低功耗调用，用来给w模关钟
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_clk_disable(void)
{
	set_hi_crg_clkdis5_pdtw_wbbp_61m_clk_dis(1);

	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_pwrctrl_tdsbbp_clk_enable
* 功 能 : 被低功耗调用，用来给TDS开钟
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_pwrctrl_wbbp_clk_enable(void)
{
	set_hi_crg_clken5_twbbp_wbbp_61m_clk_en(1);

	return BSP_OK;
}
/*****************************************************************************
* 函数  : bsp_bbp_get_tds_sleep_time
* 功能  : Get tds sleep time
* 输入  : void
* 输出  : void
* 返回  : u32
*****************************************************************************/
u32 bsp_bbp_get_tds_sleep_time(void)
{
	unsigned long flag;
    u32 u32RegVaule = 0;

	if (HW_VER_K3V3_FPGA == bsp_version_get_board_chip_type())
	{/*K3V3 上DRX 功能暂时不可用*/
		return 0xfffffff;
	}
	/* IPC锁为归避bbp_on 连读问题,单号2013121200228*/
	bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

    u32RegVaule = get_hi_bbp_tdsdrx_slp_time_cur_slp_time_cur();

	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

    return u32RegVaule;
}

/*****************************************************************************
* 函数  : bsp_bbp_get_tdsclk_switch
* 功能  : get_tdsclk_switch
* 输入  : void
* 输出  : 0表示切换到系统时钟，1表示切换到32k时钟
* 返回  : void
*****************************************************************************/
int bsp_bbp_get_tdsclk_switch(void)
{
	u32 tds_clk_switch;
	unsigned long flag;

	if (HW_VER_K3V3_FPGA == bsp_version_get_board_chip_type())
	{/*K3V3 上DRX 功能暂时不可用*/
		return 0xffff;
	}
	/* IPC锁为归避bbp_on 连读问题,单号2013121200228*/
	bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	tds_clk_switch = get_hi_bbp_tdsdrx_clk_switch_state_clk_switch_state();/*0xfff13400+0x200*/

	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

	return (int)tds_clk_switch;
}

void bbp_wakeup_int_enable(PWC_COMM_MODE_E mode)
{
	unsigned long flag;

	if (HW_VER_K3V3_FPGA == bsp_version_get_board_chip_type())
	{/*K3V3 上DRX 功能暂时不可用*/
		return ;
	}

	/* IPC锁为归避bbp_on 连读问题,单号2013121200228*/
	bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);
    /*wake up 中断使能*/
	switch(mode){
	case PWC_COMM_MODE_LTE:
		set_hi_bbp_ltedrx_arm_sleep_int_msk_wakeup_int_msk(0x1);/**/
		break;
	case PWC_COMM_MODE_TDS:
		set_hi_bbp_tdsdrx_arm_sleep_int_msk_wakeup_int_msk(0x1);
		break;
	default:
		;
	}
	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

    return ;
}
void bbp_wakeup_int_disable(PWC_COMM_MODE_E mode)
{
	unsigned long flag;

	if (HW_VER_K3V3_FPGA == bsp_version_get_board_chip_type())
	{/*K3V3 上DRX 功能暂时不可用*/
		return ;
	}

    /*wake up 中断去使能*/
	/* IPC锁为归避bbp_on 连读问题,单号2013121200228*/
	bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	switch(mode){
	case PWC_COMM_MODE_LTE:
		set_hi_bbp_ltedrx_arm_sleep_int_msk_wakeup_int_msk(0x0);/*0xfff12000+0x40*/
		break;
	case PWC_COMM_MODE_TDS:
		set_hi_bbp_tdsdrx_arm_sleep_int_msk_wakeup_int_msk(0x0);/*0xfff13400+0x40*/
		break;
	default:
		;
	}
	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

	return ;
}

void bbp_wakeup_int_clear(PWC_COMM_MODE_E mode)
{
	unsigned long 	flag;
	unsigned int 	int_status;
	unsigned int	time_stamp;
	unsigned int	new_slice;

	if (HW_VER_K3V3_FPGA == bsp_version_get_board_chip_type())
	{/*K3V3 上DRX 功能暂时不可用*/
		return ;
	}
	/* IPC锁为归避bbp_on 连读问题,单号2013121200228*/
	bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

    /*清除wake up 中断*/
	switch(mode){
	case PWC_COMM_MODE_LTE:
		set_hi_bbp_ltedrx_arm_sleep_int_clear_wakeup_int_clear(0x1);/*0xfff12000+0x44*/
		time_stamp = bsp_get_slice_value();
		do{
			int_status = get_hi_bbp_ltedrx_arm_int_state_wakeup_int_state();/*0xfff12000+0x220*/
			new_slice = bsp_get_slice_value();

			/* 获取芯片版本号为归避bbp_on 连读问题,单号2013121200228*/
			chip_version = get_hi_version_id_version_id();

		}while((int_status != 0)&&(get_timer_slice_delta(time_stamp, new_slice) < WAKEUP_INT_CLEAR_TIMEOUT));
		if(int_status != 0){
			bbp_print_error("wakeup int cannot clear!\n");
		}
		break;
	case PWC_COMM_MODE_TDS:
		set_hi_bbp_tdsdrx_arm_sleep_int_clear_wakeup_int_clear(0x1);/*0xfff13400+0x44*/
		time_stamp = bsp_get_slice_value();
		do{
			int_status = get_hi_bbp_tdsdrx_arm_int_state_wakeup_int_state();
			new_slice = bsp_get_slice_value();

			/* 获取芯片版本号为归避bbp_on 连读问题,单号2013121200228*/
			chip_version = get_hi_version_id_version_id();

		}while((int_status != 0)&&(get_timer_slice_delta(time_stamp, new_slice) < WAKEUP_INT_CLEAR_TIMEOUT));
		if(int_status != 0){
			bbp_print_error("wakeup int cannot clear!\n");
		}
		break;
	default:
		;
	}
	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);

    return ;
}

/*****************************************************************************
* 函 数 : bbp_get_wakeup_time
* 功 能 : 用来查询睡眠超时时刻
* 输 入 : PWC_COMM_MODE_E:调用模，只包含TDS/LTE
* 输 出 : void
* 返 回 : bbp睡眠剩余时间，单位为32.768KHz时钟计数
*****************************************************************************/
static u32 bbp_get_wakeup_time(PWC_COMM_MODE_E mode)
{

	u32 sleep_t = 0;/*一共睡多长时间*/
	u32 sleeped_t = 0;/*已经睡了多长时间*/
	u32 effect = 0;/*睡眠时长是否生效*/
	unsigned long flag;
	u32 ret;
	if (HW_VER_K3V3_FPGA == bsp_version_get_board_chip_type())
	{/*K3V3 上DRX 功能暂时不可用*/
		return 0xffffffff;
	}

	/* IPC锁为归避bbp_on 连读问题,单号2013121200228*/
	bsp_ipc_spin_lock_irqsave(IPC_SEM_BBP,flag);

	switch(mode){
	case PWC_COMM_MODE_LTE:

		/*判断睡眠时长是否有效,0xfff12000+0x200*/
		effect = get_hi_bbp_ltedrx_clk_switch_state_clk_switch_state();

		if(0==effect){/*若睡眠时长无效，则返回最大值*/
			ret = 0xffffffff;
			goto out;
		}

		sleep_t += get_hi_bbp_ltedrx_wakeup_32k_cnt_wakeup_32k_cnt();/*0xfff12000+0x1c*/

		/* 获取芯片版本号为归避bbp_on 连读问题,单号2013121200228*/
		chip_version = get_hi_version_id_version_id();

		sleep_t += get_hi_bbp_ltedrx_switch_32k_cnt_switch_32k_cnt();/*0xfff12000+0x20*/

		/* 获取芯片版本号为归避bbp_on 连读问题,单号2013121200228*/
		chip_version = get_hi_version_id_version_id();

		sleeped_t =get_hi_bbp_ltedrx_slp_time_cur_slp_time_cur();/*0xfff12000+0x20c*/
		break;
	case PWC_COMM_MODE_TDS:

		/*判断睡眠时长是否有效*/
		effect = get_hi_bbp_tdsdrx_clk_switch_state_clk_switch_state();/*0xfff13400+0x200*/

		if(0==effect){/*若睡眠时长无效，则返回最大值*/

			ret = 0xffffffff;
			goto out;

		}

		sleep_t += get_hi_bbp_tdsdrx_wakeup_32k_cnt_wakeup_32k_cnt();/*0xfff13400+0x1c*/

		/* 获取芯片版本号为归避bbp_on 连读问题,单号2013121200228*/
		chip_version = get_hi_version_id_version_id();

		sleep_t += get_hi_bbp_tdsdrx_switch_32k_cnt_switch_32k_cnt();/*0xfff13400+0x20*/

		/* 获取芯片版本号为归避bbp_on 连读问题,单号2013121200228*/
		chip_version = get_hi_version_id_version_id();

		sleeped_t =get_hi_bbp_tdsdrx_slp_time_cur_slp_time_cur();/*0xfff13400+0x20c*/
		break;
	default:
		;
	}
	ret = sleep_t-sleeped_t;

out:

	bsp_ipc_spin_unlock_irqrestore(IPC_SEM_BBP,flag);
	return ret;
}

/*****************************************************************************
* 函 数 : bsp_bbp_get_wakeup_time
* 功 能 : 被低功耗调用，用来查询睡眠超时时刻
* 输 入 : void
* 输 出 : void
* 返 回 : bbp睡眠剩余时间，单位为32.768KHz时钟计数
* 说 明 : 低功耗不关心哪个模还有多长时间被告唤醒，
		    只需要返回最小的时间即可
*****************************************************************************/
u32 bsp_bbp_get_wakeup_time(void)
{
	u32 l_time=0;
	u32 t_time=0;

	l_time = bbp_get_wakeup_time(PWC_COMM_MODE_LTE);
	t_time = bbp_get_wakeup_time(PWC_COMM_MODE_TDS);
	return l_time>t_time?t_time:l_time;
}
/*****************************************************************************
* 函 数 : bsp_bbp_get_gubbp_wakeup_status
* 功 能 : 被低功耗调用，用来查询GUBBP是否允许睡眠
* 输 入 : void
* 输 出 : void
* 返 回 : 0允许睡眠，非0不允许睡眠
* 说 明 : 
*****************************************************************************/
u32 bsp_bbp_get_gubbp_wakeup_status(void)
{
	return hi_get_gubbp_wakeup_status();
}

/*****************************************************************************
* 函 数 : bsp_bbp_lps_get_ta
* 功 能 : 被lps调用,查看上行子帧头相对系统子帧头的提前量
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
u16 bsp_bbp_lps_get_ta(void)
{
	return (u16)get_hi_bbp_ul_ul_ta_reg_nta_value();
}

/*用于低功耗时，临时存储0x420寄存器的值*/
u32 g_temp_420 = 0;

/*用于bbc部分时钟和复位开关控制，联调后会去掉*/
u32 g_bbc_reset = 0;

/*用于bbp部分的备份和保存的时间戳调试*/
u32 g_bbp_timer_debug = 0;

#ifdef BBC_SAVE_MEMCPY
/*低功耗调测用，0为dma方式，1为memcpy方式*/
u32 g_bbp_save_mode = 0;

const BBP_BBC_REG_STRU g_bbc_reg_list[] =
{
    {HI_BBP_CTU_C0_PRIMARY_MODE_IND_OFFSET,		0x04},
    {HI_BBP_CTU_AFC0_VALUE_INTER_RAT_OFFSET,	0x04},
    {HI_BBP_CTU_DSP_RFIC0_RSTN_OFFSET,			0x04},
    {HI_BBP_CTU_C1_PRIMARY_MODE_IND_OFFSET,		0x04},
    {HI_BBP_CTU_AFC1_VALUE_INTER_RAT_OFFSET,	0x04},
    {HI_BBP_CTU_DSP_RFIC1_RSTN_OFFSET,			0x04},
    {HI_BBP_CTU_DELAY_CNT_OFFSET,				0x08},
    {HI_BBP_CTU_LTE_MUX_FRAME_NUM_OFFSET,		0x0c},
    {HI_BBP_CTU_RFIC_CK_CFG_OFFSET,				0x04},/*start1,此处向下有101个寄存器，连续存则为205个*/
    {HI_BBP_CTU_DSP_WAITTIME_OFFSET,			0x04},
    {HI_BBP_CTU_W_PREINT_OFFSET_TIME_OFFSET,	0x08},
    {HI_BBP_CTU_INT_MASK_W_RCV_LTE_OFFSET,		0x18},
    {HI_BBP_CTU_LTE_PREINT_OFFSET_TIME_OFFSET,	0x08},
    {HI_BBP_CTU_INT_MASK_LTE_RCV_LTE_OFFSET,	0x18},
    {HI_BBP_CTU_TDS_PREINT_OFFSET_TIME_OFFSET,	0x08},
    {HI_BBP_CTU_INT_MASK_TDS_RCV_LTE_OFFSET,	0x18},
    {HI_BBP_CTU_GM_PREINT_OFFSET_TIME_OFFSET,	0x08},
    {HI_BBP_CTU_INT_MASK_GM_RCV_LTE_OFFSET,		0x18},
    {HI_BBP_CTU_GS_PREINT_OFFSET_TIME_OFFSET,	0x08},
    {HI_BBP_CTU_INT_MASK_GS_RCV_LTE_OFFSET,		0x18},
    {HI_BBP_CTU_REG0_FOR_USE_OFFSET,			0xc0},/*end1*/
    {HI_BBP_CTU_L_CH_SW_OFFSET,					0x14},/*start2,此处向下有34个寄存器，连续保存则为47个*/
    {HI_BBP_CTU_L_TCVR_SEL0_OFFSET,				0x50},
    {HI_BBP_CTU_MIPI_UNBIND_EN_OFFSET,			0x2c},/*end2*/
    {BBP_IRM_SAVE_OFFSET,0x04},/*TODO:生成hi_irm_cs.h*/
    {HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET,	0x04},/*确认此处是否需要最后恢复*/
};
#endif
char *g_bbc_save_data;
u32 g_bbp_irm_save = 0;
u32 g_lbbp_clk = 0;
/*****************************************************************************
* 函 数 : bsp_bbp_dpm_prepare
* 功 能 :
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
static int bsp_bbp_dpm_prepare(struct dpm_device *dev)
{
	u32 config_data = 0;
	char *bbp_ctu_save = g_bbc_save_data;
	u32 time_before = 0;
	u32 time_after = 0;
#ifdef BBC_SAVE_MEMCPY
	u32 i = 0;
	u32 size = 0;
#endif

#ifdef BBC_SAVE_DMA
	if(0==get_hi_crg_clkstat5_ltebbp_pd_clk_stat()){
		set_hi_crg_clken5_ltebbp_pd_clk_en(1);
		g_lbbp_clk++;
	}
	/*save register data*/
	time_before = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);
	/*数据搬移的源地址*/
	writel(HI_CTU_BASE_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_DMA_SAVE_S_OFFSET);

	/*数据搬移的目的地址*/
	writel((u32)bbp_ctu_save,HI_BBP_DMA_BASE_ADDR+BBP_DMA_SAVE_D_OFFSET);

	/*配置搬移数据长度(字节数/4-1)、模式、使能*/
	config_data = ((BBP_CTU_DATA_SIZE/4-1)<<BBP_DMA_LEN_BIT_OFFSET) \
					|(1<<BBP_DMA_FAST_BIT_OFFSET) \
					|(1<<BBP_DMA_FAST_SOFT_BIT_OFFSET) \
					|(1<<BBP_DMA_ENABLE_BIT_OFFSET);

	writel(config_data,HI_BBP_DMA_BASE_ADDR+BBP_DMA_CONFIG_OFFSET);
	time_after = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);

	/*保存特殊寄存器*/
	g_temp_420 = *(u32 *)(HI_CTU_BASE_ADDR+HI_BBP_CTU_CPU_RX1_TX1_CKG_BYPASS_OFFSET);
	g_bbp_irm_save = *(u32 *)(HI_CTU_BASE_ADDR+BBP_IRM_SAVE_OFFSET);
#endif
#ifdef BBC_SAVE_MEMCPY
		time_before = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);
	    /* 用于保存BBC	寄存器*/
	    for (i = 0,size = 0; i < sizeof(g_bbc_reg_list)/sizeof(BBP_BBC_REG_STRU); i++)
	    {
	        memcpy((void*)(&g_bbc_save_data[size]), (void*)(HI_CTU_BASE_ADDR + g_bbc_reg_list[index].reg_start), g_bbc_reg_list[index].size);
			size = size + g_bbc_reg_list[i].size;
	    }

		#if 0
	    memcpy((void*)(g_bbc_save_data), (void*)(HI_CTU_BASE_ADDR ), 0x1000);
		g_bbp_irm_save = *(u32 *)(HI_CTU_BASE_ADDR+BBP_IRM_SAVE_OFFSET);

		#endif
		time_after = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);
#endif
	if(g_bbp_timer_debug){
		bbp_print_error("prepare begin:0x%x,prepare after:0x%x\n",time_before,time_after);
	}
	return 0;
}

/*****************************************************************************
* 函 数 : bsp_bbp_dpm_suspend
* 功 能 :
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
static int bsp_bbp_dpm_suspend(struct dpm_device *dev)
{
	u32 status = 0;
	u32 time_before = 0;
	u32 time_after = 0;

	time_before = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);
#ifdef BBC_SAVE_DMA

	status = readl(HI_BBP_DMA_BASE_ADDR +BBP_DMA_STATUS_OFFSET);

	while(status & (1<<BBP_DMA_STATUS_BIT_OFFSET)){
		status = readl(HI_BBP_DMA_BASE_ADDR +BBP_DMA_STATUS_OFFSET);
	}

	if(g_lbbp_clk&&(1==get_hi_crg_clkstat5_ltebbp_pd_clk_stat())){
		set_hi_crg_clkdis5_ltebbp_pd_clk_dis(1);
		g_lbbp_clk = 0;
	}
#endif

	if(g_bbc_reset){

		/*配置BBC复位使能*/
		set_hi_crg_srsten3_bbpcommon_2a_srst_en(1);

		/*配置BBC的时钟不使能*/
		set_hi_crg_clkdis5_bbpcommon_clk_dis(1);
	}

	time_after = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);

	if(g_bbp_timer_debug){
		bbp_print_error("suspend begin: 0x%x;  suspend over:0x%x\n",time_before,time_after);
	}
#ifndef RF_RESETN_MSAVE 
	rf_reset_to_gpio_mux();
#endif
	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_dpm_resume
* 功 能 :
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
static int bsp_bbp_dpm_resume(struct dpm_device *dev)
{
	u32 config_data = 0;
	char *bbp_ctu_save = g_bbc_save_data;
	u32 time_before = 0;
	u32 time_after = 0;

	time_before = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);
	if(g_bbc_reset){

		/*时钟使能*/
		set_hi_crg_clken5_bbpcommon_2a_clk_en(1);

		/*解复位*/
		set_hi_crg_srstdis3_bbpcommon_2a_pd_srst_dis(1);
	}
#ifdef BBC_SAVE_DMA
	/*DMA模式*/
	if(0==get_hi_crg_clkstat5_ltebbp_pd_clk_stat()){
		set_hi_crg_clken5_ltebbp_pd_clk_en(1);
		g_lbbp_clk++;
	}

	/*将0x420寄存器的数值*/
	*(u32 *)(bbp_ctu_save+0x420) = 0;

	/*配置BBP的起始地址*/
	writel(HI_CTU_BASE_ADDR,HI_BBP_DMA_BASE_ADDR+BBP_DMA_SAVE_S_OFFSET);

	/*配置soc 起始地址*/
	writel((u32)bbp_ctu_save,HI_BBP_DMA_BASE_ADDR+BBP_DMA_SAVE_D_OFFSET);

	/*配置搬移数据长度(字节数/4-1)、模式、使能*/
	config_data = ((BBP_CTU_DATA_SIZE/4-1)<<BBP_DMA_LEN_BIT_OFFSET) \
					|(0<<BBP_DMA_FAST_BIT_OFFSET) \
					|(1<<BBP_DMA_ALL_DATA_BIT_OFFSET) \
					|(1<<BBP_DMA_ENABLE_BIT_OFFSET);

	writel(config_data,HI_BBP_DMA_BASE_ADDR+BBP_DMA_CONFIG_OFFSET);
#endif
#ifdef BBC_SAVE_MEMCPY
	u32 i=0;
	u32 size = 0;
    /* 用于恢复2A分区BBP寄存器 */
    for (i = 0; i < sizeof(g_bbc_reg_list)/sizeof(BBP_BBC_REG_STRU); i++)
    {
        memcpy((void*)(HI_CTU_BASE_ADDR + g_bbc_reg_list[i].reg_start), (void*)(&g_bbc_save_data[size]), g_bbc_reg_list[i].size);
        size = size + g_bbc_reg_list[i].size;
    }

	#if 0
	/*保存0x420寄存器的数值*/
	g_temp_420 = *(u32 *)(bbp_ctu_save+0x420);
	*(u32 *)(bbp_ctu_save+0x420) = 0;

	memcpy((void*)(HI_CTU_BASE_ADDR), (void*)(g_bbc_save_data),0x1000);

	*(u32 *)(HI_CTU_BASE_ADDR+BBP_IRM_SAVE_OFFSET) = g_bbp_irm_save;

	*(u32 *)(HI_CTU_BASE_ADDR+0x420)=g_temp_420;

	#endif
#endif
	time_after = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);

	if(g_bbp_timer_debug){
		bbp_print_error("resume begin: 0x%x;  resume over:0x%x\n",time_before,time_after);
	}
	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_dpm_complete
* 功 能 :
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
static int bsp_bbp_dpm_complete(struct dpm_device *dev)
{
	u32 status = 0;
	u32 time_before = 0;
	u32 time_after = 0;

	time_before = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);

#ifdef BBC_SAVE_DMA
	/*DMA模式*/

	/*查询状态，确认DMA数据搬移是否完成*/
	status = readl(HI_BBP_DMA_BASE_ADDR +BBP_DMA_STATUS_OFFSET);

	while(status & (1<<BBP_DMA_STATUS_BIT_OFFSET)){
		/*确保数据般移完成*/
		status = readl(HI_BBP_DMA_BASE_ADDR +BBP_DMA_STATUS_OFFSET);
	}

	/*写回0x420,恢复通道1上下行时钟门控bypass*/
	*(u32 *)(HI_CTU_BASE_ADDR+0x420)=g_temp_420;

	*(u32 *)(HI_CTU_BASE_ADDR+BBP_IRM_SAVE_OFFSET) = g_bbp_irm_save;

	if((1==get_hi_crg_clkstat5_ltebbp_pd_clk_stat())&&g_lbbp_clk){
		set_hi_crg_clkdis5_ltebbp_pd_clk_dis(1);

		g_lbbp_clk = 0;
	}
#endif

	time_after = bsp_get_timer_current_value(TIMER_UDELAY_TIMER_ID);
	if(g_bbp_timer_debug){
		bbp_print_error("SAVE DATA before while: 0x%x;  after while:0x%x\n",time_before,time_after);
	}
#ifndef RF_RESETN_MSAVE 
	gpio_to_rf_reset_mux();
#endif
	return BSP_OK;
}

#ifdef CONFIG_CCORE_PM
struct dpm_device g_bbp_dpm_device={
    .device_name = "bbp_dpm",
    .prepare = bsp_bbp_dpm_prepare,
    .suspend = bsp_bbp_dpm_suspend,
    .resume = bsp_bbp_dpm_resume,
    .complete = bsp_bbp_dpm_complete,
    .suspend_late = NULL,
    .resume_early =NULL
};
#endif
/*****************************************************************************
* 函 数 : bbp_timer_int_init
* 功 能 : 挂接BBP子帧中断(T 和L)
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_timer_int_init(PWC_COMM_MODE_E mode)
{
    /*mask bbp 1ms frame interrupt*/
	switch(mode){
		case PWC_COMM_MODE_LTE:
	    	set_hi_bbp_int_arm_int_msk_arm_position_int_msk(0x0);
	    	intConnect((VOIDFUNCPTR *)INT_LVL_BBPTIMER, (VOIDFUNCPTR)bbp_timerhandle, 0);
			break;
		case PWC_COMM_MODE_TDS:
	    	intConnect((VOIDFUNCPTR *)INT_LVL_TDSSTU, (VOIDFUNCPTR)bbp_tdstf_handle, 0);
			break;
		default:
			bbp_print_error("input param error:%x!\n",mode);
	}

	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bbp_dma_int_init
* 功 能 : 挂接bbp dma 中断
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_dma_int_init(void)
{
    /*enable bbp dma interrupt*/
	/*V7R2及后续芯片中，DL_DMA中断不在使用，
	   为了上层在V9R1与V7R2代码统一，此处提供桩接口即可*/

	return BSP_OK;
}

/*****************************************************************************
* 函 数 : bsp_bbp_dma_int_enable
* 功 能 : 使能bbp dma 中断
* 输 入 : void
* 输 出 : void
* 返 回 : 0为成功，-1失败
* 说 明 :
*****************************************************************************/
int bsp_bbp_dma_int_enable(void)
{
#if 0
	/*V7R2及后续芯片中，DL_DMA中断不在使用，
	   为了上层在V9R1与V7R2代码统一，此处提供桩接口即可*/

    set_hi_bbp_int_arm_int_msk_arm_dl_dma_int_msk(0x1);
    intEnable(INT_LVL_BBPDLTB);
#endif

	return 0;
}

static void bbp_tcxo_sel(void)
{
	u32 ret;
	DRV_TCXO_SEL_PARA_STRU tcxo_sel;

	/* 根据NV项配置，如果读错误，不配置，取默认值 */
	ret = bsp_nvm_read(NV_ID_DRV_TCXO_SEL, (u8*)&tcxo_sel, sizeof(DRV_TCXO_SEL_PARA_STRU));
	if(ret)
	{
		bsp_trace(BSP_LOG_LEVEL_ERROR, BSP_MODU_ABB, "[%s]: bsp_nvm_read error, ret %x\n", __FUNCTION__, ret);
	}
	else
	{
		if(0 == tcxo_sel.ucBBPCh0TcxoSel)
		{
			set_hi_crg_clk_sel3_ch0_tcxo_clk_sel(0);
		}
		else
		{
			set_hi_crg_clk_sel3_ch0_tcxo_clk_sel(1);
		}

		if(0 == tcxo_sel.ucBBPCh1TcxoSel)
		{
			set_hi_crg_clk_sel3_ch1_tcxo_clk_sel(0);
		}
		else
		{
			set_hi_crg_clk_sel3_ch1_tcxo_clk_sel(1);
		}
	}
}
int bbp_poweron(void)
{
	int ret;

	/*调用G1 BBP上电流程*/
	ret = bsp_bbp_pwrctrl_g1bbp_poweron();
	if(ret){
		bbp_print_error("fail to power on g1bbp!\n");
	}

	/*调用G2 BBP上电流程*/
	ret = bsp_bbp_pwrctrl_g2bbp_poweron();
	if(ret){
		bbp_print_error("fail to power on g2bbp!\n");
	}

	/*调用TW BBP上电流程*/
	ret = bsp_bbp_pwrctrl_twbbp_poweron();
	if(ret){
		bbp_print_error("fail to power on twbbp!\n");
	}

	/*调用W BBP上电流程*/
	ret = bsp_bbp_pwrctrl_wbbp_poweron();
	if(ret){
		bbp_print_error("fail to power on wbbp!\n");
	}

	/*调用IRM BBP上电流程*/
	ret = bbp_pwrctrl_irm_poweron();
	if(ret){
		bbp_print_error("fail to power on bbp irm!\n");
	}

	/*调用Lbbp 上电流程*/
	ret = bsp_bbp_pwrctrl_ltebbp_poweron();
	if(ret){
		bbp_print_error("fail to power on lbbp!\n");
	}
	/*调用tds开钟接口*/
	bsp_bbp_pwrctrl_tdsbbp_clk_enable();

	/*调用wbbp开钟接口*/
	bsp_bbp_pwrctrl_wbbp_clk_enable();

	return ret;
}
int bbp_poweroff(void)
{
	int ret;

	/*调用LTE BBP下电流程,TODO: 此处是否需要*/
	bsp_bbp_pwrctrl_ltebbp_poweroff();

	/*调用G1 BBP下电流程*/
	ret = bsp_bbp_pwrctrl_g1bbp_poweroff();
	if(ret){
		bbp_print_error("fail to power off g1bbp!\n");
	}

	/*调用G2 BBP下电流程*/
	ret = bsp_bbp_pwrctrl_g2bbp_poweroff();
	if(ret){
		bbp_print_error("fail to power off g2bbp!\n");
	}

	/*调用TW BBP下电流程*/
	ret = bsp_bbp_pwrctrl_twbbp_poweroff();
	if(ret){
		bbp_print_error("fail to power off twbbp!\n");
	}

	/*调用W BBP下电流程*/
	ret = bsp_bbp_pwrctrl_wbbp_poweroff();
	if(ret){
		bbp_print_error("fail to power off wbbp!\n");
	}

	/*调用IRM BBP下电流程*/
	ret = bbp_pwrctrl_irm_poweroff();
	if(ret){
		bbp_print_error("fail to power off bbp irm!\n");
	}

	/*调用tds关钟接口*/
	bsp_bbp_pwrctrl_tdsbbp_clk_disable();

	/*调用wbbp关钟接口*/
	bsp_bbp_pwrctrl_wbbp_clk_disable();

	return ret;
}

/*****************************************************************************
* 函 数 : bbp_int_init
* 功 能 :
* 输 入 : void
* 输 出 : void
* 返 回 :
* 说 明 :
*****************************************************************************/
int bbp_int_init(void)
{
	int i		= 0;
	int ret 	= BSP_OK;
	u32 chip_ver= 0;
#ifdef BSP_CONFIG_HI3630
	u8 *buf_temp =NULL;
#endif
    bbp_print_info("bbp_int_init begin\n",0,1,2,3,4,5);

	bbp_tcxo_sel();


	for(i=0;i<BBP_BUTT;i++){
		 spin_lock_init(&bbp_power_ctrl[i]);
	}

	g_bbc_save_data = cacheDmaMalloc(BBP_CTU_DATA_SIZE);
	if(NULL == g_bbc_save_data){
		bbp_print_error("cacheDmaMalloc for g_bbc_save_data failed!\n");
	}

#ifdef CONFIG_CCORE_PM
	ret = bsp_device_pm_add(&g_bbp_dpm_device);
	if (ret){
		bbp_print_error("fail to add bbp dpm device\r\n");
	}
#endif

	/*打开BBP_COMMON的时钟*/
	set_hi_crg_clken5_bbpcommon_2a_clk_en(1);

	/*打开BBP_AO的时钟*/
	set_hi_crg_clken5_bbp_ao_clk_en(1);

	/*get chip type*/
	chip_ver=bsp_version_get_board_chip_type();
	switch(chip_ver){
		/*初始化时对各模BBP 及BBP2B区上电、开钟*/
		case HW_VER_K3V3_UDP:/*回片后需要去掉*/
			//bbp_poweron();/*k3v3*/
			break;
		/*初始化时对各模BBP 及BBP2B区正电、关钟*/
		case HW_VER_PRODUCT_E5379:
		case HW_VER_PRODUCT_UDP:
			bbp_poweroff();
			break;
		case HW_VER_V711_UDP:
			bbp_poweron();/*v711,回片验证后再去掉*/
			break;
		/*不操作时时钟和电*/
		default:
		/*HW_VER_K3V3_FPGA*/
			;
	}
#ifdef BSP_CONFIG_HI3630
    buf_temp       = bsp_dump_register_field((u32)DUMP_SAVE_MOD_RUN_TRACE, DUMP_EXT_RUN_TRACE_SIZE);
	memset(buf_temp,0,DUMP_EXT_RUN_TRACE_SIZE);
	
	TdsIntRegLog   = (u32 *)buf_temp;
	TdsIntSliceLog = (u32 *)(buf_temp + DUMP_EXT_RUN_TRACE_SIZE/2);
#endif
	bbp_print_error("bbp_int_init OK\n",0,1,2,3,4,5);
    return ret;
}


