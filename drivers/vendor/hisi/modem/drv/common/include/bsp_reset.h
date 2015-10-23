

#ifndef __BSP_RESET_H__
#define __BSP_RESET_H__

#ifdef __cplusplus /* __cplusplus */
extern "C"
{
#endif /* __cplusplus */
#include <osl_common.h>
#include <bsp_memmap.h>
#include <drv_reset.h>
#include <bsp_ipc.h>
#include <bsp_shared_ddr.h>
#include <bsp_hardtimer.h>
#define RESET_OK                       (0)
#define RESET_ERROR                    (-1)
#undef  RESET_LOAD_MODEM_IMG_USE_SECOS
#define reset_debuging

#define RESET_STAGE_GET(reset_info)      ((reset_info) & 0xff)
#define RESET_ACTION_GET(reset_info)     (((reset_info)>>16) & 0xff)
#define RESET_INFO_MAKEUP(action, stage) ((action << 16) | (stage))

enum RESET_SYSFS_VALUE
{
	BALONG_MODEM_OFF   = 2,
	BALONG_MODEM_ON    = 6,
	BALONG_MODEM_RESET = 10
};

enum MODEM_RESET_BOOT_MODE
{
	CCORE_BOOT_NORMAL = 0,
	CCORE_IS_REBOOT   = 0x87654321
};

enum DRV_RESET_CB_PIOR
{
	DRV_RESET_CB_PIOR_MIN = 0,
	DRV_RESET_CB_PIOR_ICC = 0,
	DRV_RESET_CB_PIOR_IPF = 5,
	// RESET_CALLCBFUN_PIOR_<组件名>
	//...
	DRV_RESET_CB_PIOR_CSHELL = 16,
	DRV_RESET_CB_PIOR_RFILE,
	DRV_RESET_CB_PIOR_RDR = 50,
	DRV_RESET_CB_PIOR_ALL = 51,
	DRV_RESET_CB_PIOR_MAX
};

enum MODEM_ACTION
{
	MODEM_POWER_OFF,
	MODEM_POWER_ON,
	MODEM_RESET,
	MODEM_NORMAL,
	INVALID_MODEM_ACTION
};


/* M核和C核之间核间消息约定: 复位阶段信息复用DRV_RESET_CALLCBFUN_MOMENT枚举定义 */
enum RESET_ICC_MSG
{
	RESET_MCORE_BEFORE_RESET_OK,
	RESET_MCORE_RESETING_OK,
	RESET_MCORE_AFTER_RESET_OK,
	RESET_MCORE_BEFORE_MODEM_GLOBAL_BUS_ERR,
	RESET_MCORE_BEFORE_MODEM_BUS_ERR,
	RESET_MCORE_BEFORE_MODEM_ACCESS_DDRC_ERR,
	RESET_MCORE_BEFORE_NOC_POWER_IDLE_ERR,
	RESET_MCORE_BEFORE_NOC_POWER_IDLEACK_ERR,
	RESET_CCORE_REBOOT_OK,
	RESET_ACORE_MASTER_IDLE_REQ,
	RESET_CCORE_MASTER_IN_IDLE,
	RESET_INNER_CORE_INVALID_MSG,
};

/* 标识可否向对方核发送核间消息 */
enum RESET_MULTICORE_CHANNEL_STATUS
{
	CCORE_STATUS    = 0x00000001,
	MCORE_STATUS    = 0x00000002,
	HIFI_STATUS     = 0x00000004,
	INVALID_STATUS  = 0x10000000
};

enum RESET_CBFUNC_PRIO
{
	RESET_CBFUNC_PRIO_LEVEL_LOWT = 0,
	RESET_CBFUNC_PRIO_LEVEL_HIGH = 51
};

struct inter_multicore_msg
{
	u32 reset_stage;
	u32 modem_action;
};

#define ccore_msg_switch_on(flag, status)    \
do{ 	\
	(flag)  =  (flag) | (status); \
}while(0)

#define ccore_msg_switch_off(flag, status)    \
do{ 	\
	(flag)  =  (flag) & (~(status)); \
}while(0)

#ifdef CONFIG_BALONG_MODEM_RESET

#define SCBAKDATA13          (HI_AP_SYSCTRL_BASE_ADDR_VIRT + 0x348)
#define bsp_reset_bootflag_set(value)    \
do{ 	\
	writel((value), (volatile void *)SCBAKDATA13); \
}while(0)

static inline u32 bsp_reset_ccore_is_reboot(void)
{
#ifdef __KERNEL__
	return (u32)(CCORE_IS_REBOOT == (readl((const volatile void *)SCBAKDATA13))? 1: 0);
#else
	return (u32)(CCORE_IS_REBOOT == (readl(SCBAKDATA13))? 1: 0);
#endif
}

#else /* !CONFIG_BALONG_MODEM_RESET */

#define bsp_reset_bootflag_set(value)((value)=(value))
static inline u32 bsp_reset_ccore_is_reboot(void)
{
	return 0;
}

#endif /* end of CONFIG_BALONG_MODEM_RESET */

/*共享内存中保存MODEM单独复位流程中的时间戳*/
/*以下用于CCPU 复位流程时间戳*/
#define STAMP_RESET_BASE_ADDR                 (SHM_MEM_CCORE_RESET_ADDR)
#define STAMP_RESET_FIQ_COUNT                 (0x4 + STAMP_RESET_BASE_ADDR) /*用于FIQ次数*/
#define STAMP_RESET_IDLE_FAIL_COUNT           (0x4 + STAMP_RESET_FIQ_COUNT) /*用于master idle失败次数*/
#define STAMP_RESET_MASTER_ENTER_IDLE         (0x4 + STAMP_RESET_IDLE_FAIL_COUNT)
#define STAMP_RESET_CIPHER_SOFT_RESET         (0x4 + STAMP_RESET_MASTER_ENTER_IDLE)
#define STAMP_RESET_CIPHER_DISABLE_CHANNLE    (0x4 + STAMP_RESET_CIPHER_SOFT_RESET)
#define STAMP_RESET_CIPHER_ENTER_IDLE         (0x4 + STAMP_RESET_CIPHER_DISABLE_CHANNLE)
#define STAMP_RESET_EDMA_STOP_BUS             (0x4 + STAMP_RESET_CIPHER_ENTER_IDLE)
#define STAMP_RESET_EDMA_ENTER_IDLE           (0x4 + STAMP_RESET_EDMA_STOP_BUS)
#define STAMP_RESET_UPACC_ENTER_IDLE_1        (0x4 + STAMP_RESET_EDMA_ENTER_IDLE)
#define STAMP_RESET_UPACC_ENTER_IDLE_2        (0x4 + STAMP_RESET_UPACC_ENTER_IDLE_1)
#define STAMP_RESET_UPACC_ENTER_IDLE_3        (0x4 + STAMP_RESET_UPACC_ENTER_IDLE_2)
#define STAMP_RESET_CICOM0_SOFT_RESET         (0x4 + STAMP_RESET_UPACC_ENTER_IDLE_3)
#define STAMP_RESET_CICOM1_SOFT_RESET         (0x4 + STAMP_RESET_CICOM0_SOFT_RESET)
#define STAMP_RESET_IPF_SOFT_RESET            (0x4 + STAMP_RESET_CICOM1_SOFT_RESET)
#define STAMP_RESET_IPF_ENTER_IDLE            (0x4 + STAMP_RESET_IPF_SOFT_RESET)
#define STAMP_RESET_BBP_DMA_ENTER_IDLE        (0x4 + STAMP_RESET_IPF_ENTER_IDLE)
#define STAMP_RESET_WBBP_MSTER_STOP           (0x4 + STAMP_RESET_BBP_DMA_ENTER_IDLE)
#define STAMP_RESET_WBBP_SLAVE_STOP           (0x4 + STAMP_RESET_WBBP_MSTER_STOP)
#define STAMP_RESET_WBBP_ENTER_IDLE           (0x4 + STAMP_RESET_WBBP_SLAVE_STOP)
#define STAMP_RESET_MASTER_IDLE_QUIT          (0x4 + STAMP_RESET_WBBP_ENTER_IDLE)

/*以下用于M3单独复位流程时间戳*/
#define STAMP_RESET_M3_BASE_ADDR              (0x4 + STAMP_RESET_MASTER_IDLE_QUIT)
#define STAMP_RESET_M3_BUSERROR_STEP1         (0x4 + STAMP_RESET_M3_BASE_ADDR)
#define STAMP_RESET_M3_BUSERROR_STEP2         (0x4 + STAMP_RESET_M3_BUSERROR_STEP1)
#define STAMP_RESET_M3_BUSERROR_STEP3         (0x4 + STAMP_RESET_M3_BUSERROR_STEP2)
#define STAMP_RESET_M3_NOC_PROBE_DISABLE      (0x4 + STAMP_RESET_M3_BUSERROR_STEP3)
#define STAMP_RESET_M3_NOC_ENTER_LPMODE_CONFIG (0x4 + STAMP_RESET_M3_NOC_PROBE_DISABLE)
#define STAMP_RESET_M3_NOC_ENTER_LPMODE_STEP1 (0x4 + STAMP_RESET_M3_NOC_ENTER_LPMODE_CONFIG)
#define STAMP_RESET_M3_NOC_ENTER_LPMODE_STEP2 (0x4 + STAMP_RESET_M3_NOC_ENTER_LPMODE_STEP1)
#define STAMP_RESET_M3_CLEAN_NMI              (0x4 + STAMP_RESET_M3_NOC_ENTER_LPMODE_STEP2)
#define STAMP_RESET_M3_RESET_SUBSYS           (0x4 + STAMP_RESET_M3_CLEAN_NMI)

/*以下用于M3单独解复位流程时间戳*/
#define STAMP_UNRESET_M3_BASE_ADDR              (0x4 + STAMP_RESET_M3_RESET_SUBSYS)
#define STAMP_UNRESET_M3_UNRESET_SUBSYS         (0x4 + STAMP_UNRESET_M3_BASE_ADDR)
#define STAMP_UNRESET_M3_A9_STAR_ADDR           (0x4 + STAMP_UNRESET_M3_UNRESET_SUBSYS)
#define STAMP_UNRESET_M3_UNRESET_A9             (0x4 + STAMP_UNRESET_M3_A9_STAR_ADDR)

/*****************************************************************************
 函 数 名  : bsp_reset_ccpu_status_get
 功能描述  : 用于判别C核当前是否可用，可用于判断是否可以发送核间消息。A核和M3都可以使用。
 输入参数  : 无
 输出参数  : 无
 返 回 值  : int,1:可用，可以与C核交互; 0: 不可用
*****************************************************************************/
s32 bsp_reset_ccpu_status_get(void);

/*****************************************************************************
 函 数 名  : bsp_reset_init
 功能描述  : reset模块初始化
 输入参数  : 无
 输出参数  : 无
 返 回 值  : s32, RESET_OK: 初始化成功；其他值: 初始化失败
*****************************************************************************/
s32 bsp_reset_init(void);

/*****************************************************************************
 函 数 名  : bsp_modem_master_enter_idle
 功能描述  : 配置modem master进入IDLE
 输入参数  : 无
 输出参数  : 无
 返 回 值  :
*****************************************************************************/
void bsp_modem_master_enter_idle(void);

/*****************************************************************************
 函 数 名  : bsp_reset_cb_func_register
 功能描述  : 用于底软注册回调函数，处理Modem复位前后
 输入参数  : const char *pname, 组件注册的名字
         pdrv_reset_cbfun cbfun,    组件注册的回调函数
         int userdata,组件的私有数据
         Int Priolevel, 回调函数调用优先级 0-49，其中0-9 保留。使用枚举 enum DRV_RESET_CB_PIOR
 输出参数  : 无
 返 回 值  : s32, RESET_OK: 初始化成功；其他值: 初始化失败
*****************************************************************************/
s32 bsp_reset_cb_func_register(const char *name, pdrv_reset_cbfun func, int user_data, int prior);

void bsp_modem_power_off(void);
void bsp_modem_power_on(void);
int bsp_modem_reset(void);
u32 bsp_reset_is_feature_on(void);
u32 bsp_reset_is_connect_ril(void);

/*----------------------------------------------*
 * 外部函数原型说明                             *
 *----------------------------------------------*/
extern void FIQ_SysInt (void);
extern void FIQ_SysIrqEn(void);
extern void FIQ_SysIrqDis (void);

#ifdef __cplusplus /* __cplusplus */
}
#endif /* __cplusplus */

#endif    /*  __BSP_RESET_H__ */
